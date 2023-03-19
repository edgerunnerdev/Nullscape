// Copyright 2023 Pedro Nunes
//
// This file is part of Hyperscape.
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
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#include "entity/components/weaponcomponent.hpp"

#include <imgui/imgui.h>
#include <genesis.h>
#include <render/debugrender.h>
#include <scene/scene.h>

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"
#include "sector/sector.h"
#include "game.hpp"

namespace Hyperscape
{

WeaponComponent::WeaponComponent()
    : m_WeaponType( WeaponType::Turret )
    , m_Offset(0.0f)
    , m_DebugRender(false)
{
}

WeaponComponent::~WeaponComponent()
{

}

void WeaponComponent::Update(float delta)
{

}

void WeaponComponent::UpdateDebugUI()
{
    ImGui::Checkbox("Debug render", &m_DebugRender);
    
    float v[3] = {m_Offset.x, m_Offset.y, m_Offset.z};
    ImGui::InputFloat3("Offset", v);
    m_Offset = glm::vec3(v[0], v[1], v[2]);
}

void WeaponComponent::Render() 
{
    using namespace Genesis;
    if (m_DebugRender)
    {
        glm::mat4x4 transform(1.0f);

        TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
        if (pTransformComponent)
        {
            transform = pTransformComponent->GetTransform() * glm::translate(m_Offset);
        }

        Genesis::FrameWork::GetDebugRender()->DrawCross(glm::vec3(transform[3]), 10.0f, glm::vec3(1.0f));
    }
}

bool WeaponComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    Component::Serialize(data, "offset", m_Offset);
    return success;
}

bool WeaponComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    success &= TryDeserialize(data, "offset", m_Offset);
    return success;
}

void WeaponComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
    WeaponComponent* pOtherComponent = reinterpret_cast<WeaponComponent*>(pComponent);
    m_Offset = pOtherComponent->m_Offset;
}

} // namespace Hyperscape
