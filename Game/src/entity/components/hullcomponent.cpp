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

#include "entity/components/hullcomponent.hpp"

#include "entity/entity.hpp"

#include <genesis.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Hyperscape
{

HullComponent::HullComponent()
    : m_Version( 1 )
    , m_HullType( HullType::Gunship )
    , m_MaximumHitPoints( 100 )
    , m_CurrentHitPoints( 100 )
{
}

void HullComponent::UpdateDebugUI()
{
    ImGui::InputInt( "Maximum hit points", &m_MaximumHitPoints );
    ImGui::InputInt( "Current hit points", &m_CurrentHitPoints );
}

void HullComponent::Update( float delta )
{
}

bool HullComponent::Serialize( nlohmann::json& data )
{
    bool success = Component::Serialize( data );
    data[ "hull_type" ] = magic_enum::enum_name( m_HullType );
    data[ "maximum_hit_points" ] = m_MaximumHitPoints;
    data[ "current_hit_points" ] = m_CurrentHitPoints;
    return success;
}

bool HullComponent::Deserialize( const nlohmann::json& data )
{
    bool success = Component::Deserialize( data );
    success &= TryDeserialize( data, "hull_type", m_HullType );
    success &= TryDeserialize( data, "maximum_hit_points", m_MaximumHitPoints );
    success &= TryDeserialize( data, "current_hit_points", m_CurrentHitPoints );
    return success;
}

void HullComponent::CloneFrom( Component* pComponent )
{
    Component::CloneFrom( pComponent );
    HullComponent* pHullComponent = reinterpret_cast<HullComponent*>( pComponent );
    m_HullType = pHullComponent->m_HullType;
    m_MaximumHitPoints = pHullComponent->m_MaximumHitPoints;
    m_CurrentHitPoints = pHullComponent->m_CurrentHitPoints;
}

} // namespace Hyperscape
