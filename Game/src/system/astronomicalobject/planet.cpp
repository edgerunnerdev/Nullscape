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

#include "system/astronomicalobject/planet.hpp"

#include "system/astronomicalobject/orbit.hpp"

namespace Hyperscape
{

Planet::Planet(SystemRandomEngine& randomEngine, const glm::vec2& coordinates)
    : AstronomicalObject(randomEngine, "Planet", coordinates)
{
}

Planet::Planet(SystemRandomEngine& randomEngine, OrbitUniquePtr pOrbit, float theta) 
    : AstronomicalObject(randomEngine, "Planet", std::move(pOrbit), theta) 
{
}

Planet::~Planet() {}

void Planet::DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset)
{
    AstronomicalObject::DebugRender(canvasTopLeft, canvasBottomRight, canvasOffset);

    ImVec2 center = ToCanvasCoordinates(canvasTopLeft, canvasBottomRight, canvasOffset, GetCoordinates());
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddCircleFilled(center, 8.0f, IM_COL32(200, 200, 200, 255));
}

void Planet::UpdateDebugUI() 
{
    if (m_pOrbit != nullptr)
    {
        ImGui::Text("Orbital eccentricity: %.4f", m_pOrbit->GetEccentricity());
    }
}

} // namespace Hyperscape