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

#include "system/astronomicalobject/astronomicalobject.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/trigonometric.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "system/astronomicalobject/orbit.hpp"

namespace Hyperscape
{

AstronomicalObject::AstronomicalObject(SystemRandomEngine& randomEngine, const std::string& name, const glm::vec2& coordinates)
    : m_Coordinates(coordinates)
    , m_Name(name)
{
}

AstronomicalObject::AstronomicalObject(SystemRandomEngine& randomEngine, const std::string& name, OrbitUniquePtr pOrbit, float theta) 
    : m_pOrbit(std::move(pOrbit))
    , m_Name(name)
{
    m_Coordinates = m_pOrbit->At(theta);
}

AstronomicalObject::~AstronomicalObject()
{
}

void AstronomicalObject::DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight)
{
    if (m_pOrbit != nullptr)
    {    
        const float oneDegree = glm::radians(1.0f);
        for (int i = 0; i < m_OrbitPoints.size(); ++i)
        {
            m_OrbitPoints[i] = ToCanvasCoordinates(canvasTopLeft, canvasBottomRight, m_pOrbit->At(i * oneDegree));
        }
        ImGui::GetWindowDrawList()->AddPolyline(m_OrbitPoints.data(), m_OrbitPoints.size(), IM_COL32(200, 200, 200, 255), ImDrawFlags_Closed, 1.0f);
    }
}

void AstronomicalObject::UpdateDebugUI() 
{

}

const glm::vec2& AstronomicalObject::GetCoordinates() const
{
    return m_Coordinates;
}

const std::string& AstronomicalObject::GetName() const 
{
    return m_Name;
}

ImVec2 AstronomicalObject::ToCanvasCoordinates(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const glm::vec2& coordinates) const 
{
    const ImVec2 size(canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y);
    const glm::vec2 normalizedCoordinates = coordinates / 2.0f + glm::vec2(0.5f);
    return ImVec2(canvasTopLeft.x + size.x * normalizedCoordinates.x, canvasTopLeft.y + size.y * normalizedCoordinates.y);
}

} // namespace Hyperscape