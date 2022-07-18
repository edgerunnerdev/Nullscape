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

#include "system/astronomicalobject/astronomicalobject.hpp"
#include "system/system.hpp"
#include "ui2.hpp"

namespace Hyperscape
{

ExplorationViewer::ExplorationViewer()
    : m_IsOpen(false)
{
    Genesis::ImGuiImpl::RegisterMenu("Game", "Exploration viewer", &m_IsOpen);
}
   
ExplorationViewer::~ExplorationViewer() {

}

void ExplorationViewer::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        UI2::PushFont(UI2::FontId::JuraRegular20);

        using namespace ImGui;
        Begin("Sensors", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        BeginChild("System", ImVec2(1200, 600), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        DrawCanvas();
        EndChild();

        SameLine();

        BeginChild("Properties", ImVec2(300, 900), true);
        SystemSharedPtr pSystem = m_pSystem.lock();
        if (pSystem != nullptr)
        {
            if (CollapsingHeader("Seed"))
            {
                Text("Seed: %s", pSystem->GetSeed().c_str());
                
                Separator();

                static int sDepth = 1;
                InputInt("Depth", &sDepth);
                sDepth = gClamp<int>(sDepth, 1, 8);
            }

            
            if (CollapsingHeader("Astronomical objects", ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (auto& pAstronomicalObject : pSystem->GetAstronomicalObjects())
                {
                    PushID(pAstronomicalObject.get());
                    if (TreeNodeEx(pAstronomicalObject->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        pAstronomicalObject->UpdateDebugUI();
                        TreePop();
                    }
                    PopID();
                }
            }
        }
        EndChild();

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
    static ImVector<ImVec2> points;
    static ImVec2 scrolling(0.0f, 0.0f);
    static const float sSectorSize = 48.0f;
    static const size_t sNumSectorsW = 31;
    static const size_t sNumSectorsH = 31;

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImVec2(sSectorSize * static_cast<float>(sNumSectorsW), sSectorSize * static_cast<float>(sNumSectorsH));
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered();                            // Hovered
    const bool is_active = ImGui::IsItemActive();                              // Held
    const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
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
        if (ImGui::MenuItem("Test 1", NULL, false, points.Size > 0))
        {
            
        }
        if (ImGui::MenuItem("Test 2", NULL, false, points.Size > 0))
        {
            
        }
        ImGui::EndPopup();
    }

    // Draw grid + all lines in the canvas
    pDrawList->PushClipRect(canvas_p0, canvas_p1, true);
    const float GRID_STEP = sSectorSize;
    for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
        pDrawList->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
        pDrawList->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));

    SystemSharedPtr pSystem = m_pSystem.lock();
    if (pSystem != nullptr)
    {
        for (auto& pAstronomicalObject : pSystem->GetAstronomicalObjects())
        {
            pAstronomicalObject->DebugRender(canvas_p0, canvas_p1, scrolling);
        }
    }
    pDrawList->PopClipRect();

    //ImGui::EndChild();
}

} // namespace Hyperscape