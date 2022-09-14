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

#include "entity/components/trailcomponent.hpp"

#include <imgui/imgui.h>
#include <genesis.h>
#include <render/debugrender.h>

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"
#include "sector/sector.h"
#include "sprite/sprite.h"
#include "sprite/spritemanager.h"
#include "trail/trailmanager.h"
#include "trail/trail.h"
#include "game.hpp"

namespace Nullscape
{

TrailComponent::TrailComponent()
    : m_Offset(0.0f)
    , m_Width(1.0f)
    , m_Lifetime(1.0f)
    , m_Color(1.0f)
    , m_DebugRender(false)
{

}

TrailComponent::~TrailComponent()
{
    TrailSharedPtr pTrail = m_pTrail.lock();
    if (pTrail)
    {
        pTrail->SetOrphan();
    }
}

void TrailComponent::Update(float delta)
{
    if (m_pTrail.expired() && g_pGame->GetCurrentSector())
    {
        m_pTrail = g_pGame->GetCurrentSector()->GetTrailManager()->Add(m_Width, m_Lifetime, m_Color);
    }

    TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    glm::mat4x4 transform(1.0f);
    if (pTransformComponent)
    {
        transform = pTransformComponent->GetTransform() * glm::translate(m_Offset);

        TrailSharedPtr pTrail = m_pTrail.lock();
        if (pTrail)
        {
            pTrail->AddPoint(transform);
        }
    }
    else
    {
        transform = glm::translate(m_Offset);
    }

    Sprite sprite(glm::vec3(transform[3]), Genesis::Colour(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, 0);
    g_pGame->GetCurrentSector()->GetSpriteManager()->AddSprite(sprite);
}

void TrailComponent::UpdateDebugUI()
{
    ImGui::Checkbox("Debug render", &m_DebugRender);
    
    if (ImGui::InputFloat("Width", &m_Width))
    {
        m_Width = glm::max(0.1f, m_Width);
    }

    if (ImGui::InputFloat("Lifetime", &m_Lifetime))
    {
        m_Lifetime = glm::max(0.1f, m_Lifetime);
    }

    float v[3] = {m_Offset.x, m_Offset.y, m_Offset.z};
    ImGui::InputFloat3("Offset", v);
    m_Offset = glm::vec3(v[0], v[1], v[2]);
}

void TrailComponent::Render() 
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

        Genesis::FrameWork::GetDebugRender()->DrawCross(glm::vec3(transform[3]), m_Width, glm::vec3(1.0f));
    }
}

bool TrailComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    Component::Serialize(data, "offset", m_Offset);
    Component::Serialize(data, "width", m_Width);
    Component::Serialize(data, "lifetime", m_Lifetime);
    Component::Serialize(data, "color", m_Color);
    return success;
}

bool TrailComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    success &= TryDeserialize(data, "offset", m_Offset);
    success &= TryDeserialize(data, "width", m_Width);
    success &= TryDeserialize(data, "lifetime", m_Lifetime);
    success &= TryDeserialize(data, "color", m_Color);
    return success;
}

void TrailComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
    TrailComponent* pOtherComponent = reinterpret_cast<TrailComponent*>(pComponent);
    m_Offset = pOtherComponent->m_Offset;
    m_Width = pOtherComponent->m_Width;
    m_Lifetime = pOtherComponent->m_Lifetime;
    m_Color = pOtherComponent->m_Color;
}

} // namespace Nullscape
