// Copyright 2022 Pedro Nunes
//
// This file is part of Nullscape.
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
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#include "entity/components/cameracomponent.hpp"

#include <imgui/imgui.h>
#include <genesis.h>

namespace Nullscape
{

CameraComponent::CameraComponent()
    : m_Offset(0.0f)
{

}

void CameraComponent::UpdateDebugUI()
{
    float v[3] = { m_Offset.x, m_Offset.y, m_Offset.z };
    ImGui::InputFloat3("Offset", v);
    m_Offset = glm::vec3(v[0], v[1], v[2]);
}

bool CameraComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    Component::Serialize(data, "offset", m_Offset);
    return success;
}

bool CameraComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    success &= TryDeserialize(data, "offset", m_Offset);
    return success;
}

void CameraComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
    CameraComponent* pOtherComponent = reinterpret_cast<CameraComponent*>(pComponent);
    m_Offset = pOtherComponent->m_Offset;
}

} // namespace Nullscape
