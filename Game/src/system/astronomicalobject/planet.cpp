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

namespace Hyperscape
{

Planet::Planet(SystemRandomEngine& randomEngine, const glm::vec2& coordinates)
    : AstronomicalObject(randomEngine, coordinates)
{
}

Planet::~Planet() {}

void Planet::DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight) 
{
    ImVec2 size(canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y);
    glm::vec2 normalizedCoordinates = GetCoordinates() + glm::vec2(1.0f) / 2.0f;
    ImVec2 center(canvasTopLeft.x + size.x * normalizedCoordinates.x, canvasTopLeft.y + size.y * normalizedCoordinates.y);

    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddCircleFilled(center, 8.0f, IM_COL32(200, 200, 0, 255));
}

} // namespace Hyperscape