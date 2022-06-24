// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "scene/light.h"

#include <imgui/imgui.h>

namespace Genesis
{

Light::Light()
    : m_Position(100.0f, 100.0f, 100.0f)
    , m_Color(1.0f, 1.0f, 1.0f)
{
}

Light::Light(const glm::vec3& position, const glm::vec3& color)
    : m_Position(position)
    , m_Color(color)
{
}

void Light::SetPosition(const glm::vec3& position)
{
    m_Position = position;
}

const glm::vec3& Light::GetPosition() const
{
    return m_Position;
}

void Light::SetColor(const glm::vec3& color)
{
    m_Color = color;
}

const glm::vec3& Light::GetColor() const
{
    return m_Color;
}

void Light::DebugDraw() 
{
    float pos[3] = { m_Position.x, m_Position.y, m_Position.z };
    ImGui::InputFloat3("Position", pos);
    m_Position = glm::vec3(pos[0], pos[1], pos[2]);

    float color[3] = { m_Color.r, m_Color.g, m_Color.b };
    ImGui::ColorEdit3("Color", color);
    m_Color = glm::vec3(color[0], color[1], color[2]);
}

} // namespace Genesis
