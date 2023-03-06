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

#include "entity/components/shipdetailscomponent.hpp"

#include "entity/entity.hpp"

#include <genesis.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Hyperscape
{

ShipDetailsComponent::ShipDetailsComponent()
    : m_Version( 1 )
{
}

void ShipDetailsComponent::UpdateDebugUI()
{
    ImGui::InputText( "Model", &m_ShipName );
}

void ShipDetailsComponent::Update( float delta )
{
}

bool ShipDetailsComponent::Serialize( nlohmann::json& data )
{
    bool success = Component::Serialize( data );
    data[ "ship_name" ] = m_ShipName;
    return success;
}

bool ShipDetailsComponent::Deserialize( const nlohmann::json& data )
{
    bool success = Component::Deserialize( data );
    success &= TryDeserialize( data, "ship_name", m_ShipName );
    return success;
}

void ShipDetailsComponent::CloneFrom( Component* pComponent )
{
    Component::CloneFrom( pComponent );
    ShipDetailsComponent* pShipDetailsComponent = reinterpret_cast<ShipDetailsComponent*>( pComponent );
    m_ShipName = pShipDetailsComponent->m_ShipName;
}

} // namespace Hyperscape
