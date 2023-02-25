// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "viewers/explorationviewer/explorationviewer.hpp"

#include <sstream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <implot/implot.h>
#include <math/misc.h>

#include "sector/sector.h"
#include "system/astronomicalobject/astronomicalobject.hpp"
#include "system/system.hpp"
#include "ui2/ui2.hpp"

namespace Hyperscape
{

// utility structure for realtime plot
struct RollingBuffer
{
    float Span;
    ImVector<ImVec2> Data;
    RollingBuffer()
    {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void AddPoint(float x, float y)
    {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
};

ExplorationViewer::ExplorationViewer()
    : m_WindowSize(1200, 900)
    , m_IsOpen(false)
    , m_Angle(0.0f)
    , m_Aperture(45.0f)
    , m_RangeMin(0.0f)
    , m_RangeMax(2.0f)
    , m_SpectrographYMax(1.0f)
    , m_Calibration(0.0f)
    , m_CalibrationDecayTimer(0.0f)
    , m_ScanTimer(0.0f)
    , m_SignalsInArc(0)
{
    Genesis::ImGuiImpl::RegisterDevMenu("Game", "Sensors", &m_IsOpen);
}
   
ExplorationViewer::~ExplorationViewer() {

}

void ExplorationViewer::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        const float delta = ImGui::GetIO().DeltaTime;
        m_ScanTimer -= delta;
        if (m_ScanTimer <= 0.0f)
        {
            m_ScanTimer = 0.1f;
            DoScan();
        }

        using namespace ImGui;
        UI2::PushFont(UI2::FontId::ArconBold18);
        Begin("// SENSORS", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        UI2::PopFont();
        UI2::PushFont(UI2::FontId::ArconRegular18);

        int spectrographHeight = 316;
        BeginGroup();
        BeginChild("System", ImVec2(m_WindowSize.x, m_WindowSize.y - spectrographHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        DrawCanvas();
        EndChild();

        BeginChild("Spectrograph", ImVec2(m_WindowSize.x, spectrographHeight), true, ImGuiWindowFlags_NoScrollbar);
        DrawSpectrograph();
        EndChild();

        EndGroup();

        SameLine();

        BeginGroup();
        const ImVec2 signalsSize(500, m_WindowSize.y - spectrographHeight);
        BeginChild("Signals", signalsSize, true);
        SystemSharedPtr pSystem = m_pSystem.lock();
        if (pSystem != nullptr)
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("SignalsTable", 4, flags))
            {
                UI2::PushFont(UI2::FontId::ArconBold18);
                ImGui::TableSetupColumn("ID      ", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Lock", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                UI2::PopFont();

                for (auto& pSignalSource : pSystem->GetSignalSources())
                {
                    float signalLockCurrent, signalLockMaximum;
                    GetSignalLock(pSignalSource, signalLockCurrent, signalLockMaximum);
                    if (signalLockMaximum < 1.0f)
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("%s", pSignalSource->GetSignalId().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("%s", signalLockMaximum < 0.5f ? "???" : pSignalSource->GetSignalType().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("%s", signalLockMaximum < 0.5f ? "???" : pSignalSource->GetSignalName().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("%.1f", signalLockCurrent * 100.0f);
                    }
                    else
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(pSignalSource->GetSignalId().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(pSignalSource->GetSignalType().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(pSignalSource->GetSignalName().c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f", signalLockCurrent * 100.0f);
                    }
                }
                ImGui::EndTable();
            }
        }
        EndChild();

        static float sCalibrationHeight = 187.0f;
        BeginChild("Calibration", ImVec2(signalsSize.x, sCalibrationHeight), true);
        DrawCalibration();
        EndChild();

        static float sPropertiesHeight = 125.0f;
        BeginChild("Properties", ImVec2(signalsSize.x, sPropertiesHeight), true);
        const float maximumSensorRange = GetMaximumSensorRange();
        if (ImGui::SliderFloat("Angle", &m_Angle, -360.0f, 360.0f, "%.0f deg", 1.0f))
        {
            TriggerCalibrationDecay();
        }

        if (ImGui::SliderFloat("Aperture", &m_Aperture, 5.0f, 180.0f, "%.0f deg", 1.0f))
        {
            TriggerCalibrationDecay();
        }

        if (ImGui::SliderFloat("Minimum range", &m_RangeMin, 0.0f, m_RangeMax - 0.25f, "%.2f AU"))
        {
            TriggerCalibrationDecay();
        }
        
        if (m_RangeMin >= m_RangeMax - GetMinimumSensorRange())
        {
            m_RangeMin = m_RangeMax - GetMinimumSensorRange();
        }

        if (ImGui::SliderFloat("Maximum range", &m_RangeMax, 0.25f, maximumSensorRange, "%.2f AU"))
        {
            TriggerCalibrationDecay();
        }
        EndChild();
        EndGroup();

        End();

        UI2::PopFont();
    }
}

void ExplorationViewer::View(SystemSharedPtr pSystem)
{
    m_pSystem = pSystem;
}

void ExplorationViewer::DrawCanvas()
{
    static ImVec2 scrolling(0.0f, 0.0f);
    static const float sSectorSize = 48.0f;

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvasSize = ImVec2(1200, 900);
    ImVec2 topLeft = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
    ImVec2 bottomRight = ImVec2(topLeft.x + canvasSize.x, topLeft.y + canvasSize.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered();                            // Hovered
    const bool is_active = ImGui::IsItemActive();                              // Held
    const ImVec2 origin(topLeft.x + scrolling.x, topLeft.y + scrolling.y); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    const float mouse_threshold_for_pan = -1.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, mouse_threshold_for_pan))
    {
        scrolling.x += io.MouseDelta.x;
        scrolling.y += io.MouseDelta.y;
    } 

    // Context menu (under default mouse threshold)
    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context");
    if (ImGui::BeginPopup("context"))
    {
        if (ImGui::MenuItem("Test 1", NULL, false))
        {
            
        }
        if (ImGui::MenuItem("Test 2", NULL, false))
        {
            
        }
        ImGui::EndPopup();
    }

    // Draw grid + all lines in the canvas
    pDrawList->PushClipRect(topLeft, bottomRight, true);
    const float GRID_STEP = sSectorSize;
    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
        pDrawList->AddLine(ImVec2(topLeft.x + x, topLeft.y), ImVec2(topLeft.x + x, bottomRight.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
        pDrawList->AddLine(ImVec2(topLeft.x, topLeft.y + y), ImVec2(bottomRight.x, topLeft.y + y), IM_COL32(200, 200, 200, 40));

    DrawScannerArc(topLeft, bottomRight, scrolling);

    SystemSharedPtr pSystem = m_pSystem.lock();
    if (pSystem != nullptr)
    {
        for (auto& pSignalSource : pSystem->GetSignalSources())
        {
            float signalLockCurrent, signalLockMaximum;
            GetSignalLock(pSignalSource, signalLockCurrent, signalLockMaximum);
            if (signalLockMaximum >= 1.0f)
            {
                pSignalSource->CanvasRender(topLeft, bottomRight, scrolling);
            }
        }
    }
    
    pDrawList->PopClipRect();
}

void ExplorationViewer::DrawCalibration() 
{
    static RollingBuffer sRollingData;
    static float t = 0;
    const float deltaTime = ImGui::GetIO().DeltaTime;
    t += deltaTime;
    m_CalibrationDecayTimer = gMax(m_CalibrationDecayTimer - deltaTime, 0.0f);

    float rangeFactor = 1.0f - (m_RangeMax - m_RangeMin - GetMinimumSensorRange()) / (GetMaximumSensorRange() - GetMinimumSensorRange());
    float apertureFactor = 1.0f - (m_Aperture - 5.0f) / 175.0f;
    float effectivenessFactor = 0.2f + rangeFactor * 0.4f + apertureFactor * 0.4f;
    bool limitExceeded = m_Calibration > effectivenessFactor;

    std::uniform_real_distribution<float> dist(-0.75f, (/* m_CalibrationDecayTimer > 0.0f ||*/ limitExceeded) ? 0.0f : 1.0f);
    m_Calibration = gClamp(m_Calibration + ImGui::GetIO().DeltaTime * dist(m_RandomEngine), 0.0f, 1.0f);

    float sensorStrength = GetMaximumSensorStrength() * m_Calibration;
    sRollingData.AddPoint(t, sensorStrength);

    static float history = 10.0f;
    sRollingData.Span = history;

    static float sCalibrationHeight = 170.0f;
    if (ImPlot::BeginPlot("##Calibration", ImVec2(-1, sCalibrationHeight), ImPlotFlags_NoLegend | ImPlotFlags_NoInputs | ImPlotFlags_NoMenus))
    {
        ImPlot::SetupAxes(nullptr, "Sensor strength", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, history, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, GetMaximumSensorStrength() * 1.1f);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(86, 224, 199, 255));
        ImPlot::PlotShaded("Strength", &sRollingData.Data[0].x, &sRollingData.Data[0].y, sRollingData.Data.size(), 0, 0, 2 * sizeof(float));
        ImPlot::PlotLine("Strength", &sRollingData.Data[0].x, &sRollingData.Data[0].y, sRollingData.Data.size(), 0, 2 * sizeof(float));
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

void ExplorationViewer::DrawSpectrograph() 
{
    if (ImPlot::BeginPlot("Spectrograph", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoInputs | ImPlotFlags_NoMenus))
    {
        static const char* pPlotName = "Signal";
        ImPlot::SetupAxis(ImAxis_X1, "Wavelength (m)", ImPlotAxisFlags_LogScale | ImPlotAxisFlags_Lock);
        ImPlot::SetupAxis(ImAxis_Y1, "Radiance", ImPlotAxisFlags_Lock);
        ImPlot::SetupAxesLimits(0.0, m_ScanResult.GetMaximumWavelength().ToDouble(), 0.0, (m_SignalsInArc > 0) ? m_SpectrographYMax * 1.1 : 1.0, ImGuiCond_Always);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PushStyleVar(ImPlotStyleVar_TickLabelWidth, 64.0f);
        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(86, 224, 199, 255));
        ImPlot::PlotShaded(pPlotName, m_ScanResult.Wavelengths.data(), m_ScanResult.Intensities.data(), m_ScanResult.Wavelengths.size());
        ImPlot::PlotLine(pPlotName, m_ScanResult.Wavelengths.data(), m_ScanResult.Intensities.data(), m_ScanResult.Wavelengths.size());
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar(2);
        ImPlot::EndPlot();
    }
}

void ExplorationViewer::DrawScannerArc(const ImVec2& topLeft, const ImVec2& bottomRight, const ImVec2& offset)
{
    SystemSharedPtr pSystem = m_pSystem.lock();
    if (pSystem == nullptr || pSystem->GetCurrentSector() == nullptr)
    {
        return;
    }

    const glm::vec2& sectorCoordinates = pSystem->GetCurrentSector()->GetCoordinates();
    ImVec2 playerCoordinates = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, sectorCoordinates);
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddCircle(playerCoordinates, 8.0f, ImColor(0.0f, 1.0f, 1.0f));

    const float angleRad = glm::radians(m_Angle);
    const float maximumSensorRange = GetMaximumSensorRange();
    glm::vec2 angleFrom;
    glm::vec2 angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad), glm::sin(angleRad)) * maximumSensorRange;
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(86, 224, 199, 64));

    const float apertureRad = glm::radians(m_Aperture);
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * maximumSensorRange;
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(86, 224, 199, 128));

    angleFrom = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * m_RangeMin;
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * m_RangeMax;
    pDrawList->AddLine(UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleFrom), UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(86, 224, 199), 2.0f);

    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * maximumSensorRange;
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f, 0.5f));

    angleFrom = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * m_RangeMin;
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * m_RangeMax;
    pDrawList->AddLine(UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleFrom), UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(86, 224, 199), 2.0f);

    const float arcStepRad = glm::radians(1.0f);
    std::vector<ImVec2> rangeArc;
    rangeArc.resize(m_Aperture * 2 + 1);
    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * maximumSensorRange;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(86, 224, 199, 128), 0, 1.0f);


    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMin;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(86, 224, 199), 0, 2.0f);

    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMax;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(86, 224, 199), 0, 2.0f);

    pDrawList->Flags &= ~ImDrawListFlags_AntiAliasedFill;
    std::vector<ImVec2> rangeMinArc;
    std::vector<ImVec2> rangeMaxArc;
    rangeMinArc.resize(m_Aperture * 2 + 1);
    rangeMaxArc.resize(m_Aperture * 2 + 1);
    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 pointMin = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMin;
        rangeMinArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, pointMin);

        glm::vec2 pointMax = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMax;
        rangeMaxArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, pointMax);
    }

    for (int i = 0; i < m_Aperture * 2; ++i)
    {
        ImVec2 poly[4] = {rangeMinArc[i], rangeMaxArc[i], rangeMaxArc[i + 1], rangeMinArc[i + 1]};
        pDrawList->AddConvexPolyFilled(poly, 4, ImColor(86, 224, 199, 64));
    }

    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        rangeMinArc[i] = playerCoordinates;

        glm::vec2 pointMax = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * maximumSensorRange;
        rangeMaxArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, pointMax);
    }

    for (int i = 0; i < m_Aperture * 2; ++i)
    {
        ImVec2 poly[4] = {rangeMinArc[i], rangeMaxArc[i], rangeMaxArc[i + 1], rangeMinArc[i + 1]};
        pDrawList->AddConvexPolyFilled(poly, 4, ImColor(86, 224, 199, 25));
    }

    pDrawList->Flags |= ImDrawListFlags_AntiAliasedFill;
}

float ExplorationViewer::GetMinimumSensorRange() const
{
    return 0.25f;
}

float ExplorationViewer::GetMaximumSensorRange() const 
{
    return 2.0f;
}

float ExplorationViewer::GetMaximumSensorStrength() const 
{
    return 1000.0f;
}

void ExplorationViewer::DoScan() 
{
    // Reset result scan intensities
    for (size_t i = 0; i < m_ScanResult.sNumEntries; ++i)
    {
        m_ScanResult.Intensities[i] = 0.0;
    }
    m_SpectrographYMax = 0.0f;
    m_SignalsInArc = 0;

    SystemSharedPtr pSystem = m_pSystem.lock();
    if (pSystem != nullptr)
    {
        for (auto& pSignalSource : pSystem->GetSignalSources())
        {
            if (IsInScannerArc(pSignalSource->GetSignalCoordinates()))
            {
                const SignalData& signalData = pSignalSource->GetSignalData();
                for (size_t i = 0; i < m_ScanResult.sNumEntries; ++i)
                {
                    m_ScanResult.Intensities[i] += signalData.Intensities[i];
                    m_SpectrographYMax = glm::max(m_SpectrographYMax, static_cast<float>(m_ScanResult.Intensities[i]));
                }
                m_SignalsInArc++;
                UpdateSignalLock(pSignalSource, true);
            }
            else
            {
                UpdateSignalLock(pSignalSource, false);
            }
        }
    }
}

bool ExplorationViewer::IsInScannerArc(const glm::vec2& coordinates) const 
{
    SystemSharedPtr pSystem = m_pSystem.lock();
    if (pSystem == nullptr || pSystem->GetCurrentSector() == nullptr)
    {
        return false;
    }

    const glm::vec2& sectorCoordinates = pSystem->GetCurrentSector()->GetCoordinates();
    const float scannerAngleRad = glm::radians(m_Angle);
    const glm::vec2 from(glm::cos(scannerAngleRad), glm::sin(scannerAngleRad));
    glm::vec2 to(coordinates - sectorCoordinates);
    const float toDistance = glm::length(to);

    if (glm::length(to) <= 0.001) 
    {
        return true;
    }
    else
    {
        to = glm::normalize(to);
    }

    const bool isInAperture = glm::degrees(glm::acos(glm::dot(from, to))) < m_Aperture;
    const bool isInRange = toDistance > m_RangeMin && toDistance < m_RangeMax;

    return isInAperture && isInRange;
}

void ExplorationViewer::TriggerCalibrationDecay() 
{
    m_CalibrationDecayTimer = 0.5f;
}

void ExplorationViewer::UpdateSignalLock(const SignalSourceSharedPtr& pSignalSource, bool isInArc) 
{
    if (pSignalSource->GetSignalDifficulty() <= 0.0f)
    {
        m_SignalLocks[pSignalSource->GetSignalId()] = SignalLockData(1.0f, 1.0f);
    }
    else
    {
        SignalLocks::iterator it = m_SignalLocks.find(pSignalSource->GetSignalId());
        float lock = GetEffectiveSensorStrenght() / pSignalSource->GetSignalDifficulty();
        if (it != m_SignalLocks.end())
        {
            // Was this signal previous locked? If so, it should remain fully locked.
            if (it->second.maximum >= 1.0f)
            {
                return;
            }
            else if (isInArc)
            {
                it->second.current = lock;
                it->second.maximum = glm::max(it->second.maximum, lock);
            }
            else
            {
                it->second.current = 0.0f;
            }
        }
        else
        {
            m_SignalLocks[pSignalSource->GetSignalId()] = isInArc ? SignalLockData(lock, lock) : SignalLockData(0.0f, 0.0f); 
        }
    }
}

float ExplorationViewer::GetEffectiveSensorStrenght() const 
{
    return GetMaximumSensorStrength() * m_Calibration;
}

void ExplorationViewer::GetSignalLock(const SignalSourceSharedPtr& pSignalSource, float& current, float& maximum) const 
{
    SignalLocks::const_iterator it = m_SignalLocks.find(pSignalSource->GetSignalId());
    if (it == m_SignalLocks.end())
    {
        current = 0.0f;
        maximum = 0.0f;
    }
    else
    {
        current = it->second.current;
        maximum = it->second.maximum;
    }
}

} // namespace Hyperscape