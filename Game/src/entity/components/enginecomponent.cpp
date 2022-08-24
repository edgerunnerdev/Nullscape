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

#include "entity/components/enginecomponent.hpp"

#include <imgui/imgui.h>
#include <genesis.h>

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"
#include "sector/sector.h"
#include "trail/trailmanager.h"
#include "trail/trail.h"
#include "game.hpp"

namespace Nullscape
{

EngineComponent::EngineComponent()
    : m_Offset(0.0f)
    , m_Width(1.0f)
    , m_Decay(1.0f)
    , m_Color(1.0f)
{

}

EngineComponent::~EngineComponent() 
{
    if (m_pTrail && g_pGame->GetCurrentSector())
    {
        g_pGame->GetCurrentSector()->GetTrailManager()->Remove(m_pTrail.get());
    }
}

void EngineComponent::Update(float delta) 
{
    if (!m_pTrail && g_pGame->GetCurrentSector())
    {
        TrailManager* pTrailManager = g_pGame->GetCurrentSector()->GetTrailManager();
        m_pTrail = std::make_unique<Trail>(m_Width, m_Decay, m_Color);
        pTrailManager->Add(m_pTrail.get());
    }

    TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    if (m_pTrail && pTransformComponent)
    {
        m_pTrail->AddPoint(pTransformComponent->GetPosition());
    }
}

void EngineComponent::UpdateDebugUI() 
{

}

void EngineComponent::Render()
{

}

bool EngineComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    //    : m_Offset(0.0f)
    //, m_Width(1.0f)
    //, m_Decay(1.0f)
    //, m_Color(1.0f)
    
    Component::Serialize(data, "offset", m_Offset);

    // 
    //data["filename"] = m_Filename;
    return success;
}

bool EngineComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    //success &= TryDeserialize(data, "filename", m_Filename);
    return success;
}

void EngineComponent::CloneFrom(Component* pComponent) 
{
    Component::CloneFrom(pComponent);
    //ModelComponent* pModelComponent = reinterpret_cast<ModelComponent*>(pComponent);
    //m_Filename = pModelComponent->m_Filename;
}

} // namespace Nullscape
