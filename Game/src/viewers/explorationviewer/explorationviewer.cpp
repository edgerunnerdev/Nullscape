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
#include <math/misc.h>

#include "sector/sector.h"
#include "system/astronomicalobject/astronomicalobject.hpp"
#include "system/system.hpp"
#include "ui2.hpp"

namespace Hyperscape
{

ExplorationViewer::ExplorationViewer()
    : m_WindowSize(1200, 900)
    , m_IsOpen(false)
    , m_Angle(0.0f)
    , m_Aperture(45.0f)
    , m_RangeMin(0.0f)
    , m_RangeMax(2.0f)
{
    Genesis::ImGuiImpl::RegisterMenu("Game", "Sensors", &m_IsOpen);
}
   
ExplorationViewer::~ExplorationViewer() {

}

void ExplorationViewer::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {

        UI2::PushFont(UI2::FontId::ArconBold18);
        using namespace ImGui;
        Begin("Sensors", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize);
        UI2::PopFont();

        UI2::PushFont(UI2::FontId::ArconRegular18);
        BeginChild("System", m_WindowSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        DrawCanvas();
        EndChild();

        SameLine();

        BeginGroup();
        const ImVec2 signalsSize(500, 600);
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
                ImGui::TableSetupColumn("Strength", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                UI2::PopFont();

                for (auto& pSignalSource : pSystem->GetSignalSources())
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(pSignalSource->GetSignalId().c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(pSignalSource->GetSignalType().c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(pSignalSource->GetSignalName().c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("100.0%");
                }
                ImGui::EndTable();
            }
        }
        EndChild();

        BeginChild("Properties", ImVec2(signalsSize.x, m_WindowSize.y - signalsSize.y), true);
        const float maximumSensorRange = GetMaximumSensorRange();
        ImGui::SliderFloat("Angle", &m_Angle, -360.0f, 360.0f, "%.0f deg", 1.0f);
        ImGui::SliderFloat("Aperture", &m_Aperture, 5.0f, 180.0f, "%.0f deg", 1.0f);
        ImGui::SliderFloat("Minimum range", &m_RangeMin, 0.0f, m_RangeMax - 0.25f, "%.2f AU");
        if (m_RangeMin >= m_RangeMax - 0.25f)
        {
            m_RangeMin = m_RangeMax - 0.25f;
        }
        ImGui::SliderFloat("Maximum range", &m_RangeMax, 0.25f, maximumSensorRange, "%.2f AU");
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
            pSignalSource->CanvasRender(topLeft, bottomRight, scrolling);
        }
    }
    
    pDrawList->PopClipRect();
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
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f, 0.25f));

    const float apertureRad = glm::radians(m_Aperture);
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * maximumSensorRange;
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f, 0.5f));

    angleFrom = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * m_RangeMin;
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad), glm::sin(angleRad - apertureRad)) * m_RangeMax;
    pDrawList->AddLine(UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleFrom), UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f), 2.0f);

    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * maximumSensorRange;
    pDrawList->AddLine(playerCoordinates, UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f, 0.5f));

    angleFrom = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * m_RangeMin;
    angleTo = sectorCoordinates + glm::vec2(glm::cos(angleRad + apertureRad), glm::sin(angleRad + apertureRad)) * m_RangeMax;
    pDrawList->AddLine(UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleFrom), UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, angleTo), ImColor(0.0f, 1.0f, 1.0f), 2.0f);

    const float arcStepRad = glm::radians(1.0f);
    std::vector<ImVec2> rangeArc;
    rangeArc.resize(m_Aperture * 2 + 1);
    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * maximumSensorRange;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(0.0f, 1.0f, 1.0f, 0.5f), 0, 1.0f);


    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMin;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(0.0f, 1.0f, 1.0f), 0, 2.0f);

    for (int i = 0; i <= m_Aperture * 2; ++i)
    {
        glm::vec2 point = sectorCoordinates + glm::vec2(glm::cos(angleRad - apertureRad + arcStepRad * i), glm::sin(angleRad - apertureRad + arcStepRad * i)) * m_RangeMax;
        rangeArc[i] = UI2::ToCanvasCoordinates(topLeft, bottomRight, offset, point);
    }
    pDrawList->AddPolyline(rangeArc.data(), rangeArc.size(), ImColor(0.0f, 1.0f, 1.0f), 0, 2.0f);

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
        pDrawList->AddConvexPolyFilled(poly, 4, ImColor(0.0f, 1.0f, 1.0f, 0.25f));
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
        pDrawList->AddConvexPolyFilled(poly, 4, ImColor(0.0f, 1.0f, 1.0f, 0.1f));
    }

    pDrawList->Flags |= ImDrawListFlags_AntiAliasedFill;
}

float ExplorationViewer::GetMaximumSensorRange() const 
{
    return 2.0f;
}

} // namespace Hyperscape