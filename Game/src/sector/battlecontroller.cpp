// Copyright 2023 Pedro Nunes
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

#include "sector/battlecontroller.hpp"

#include "entity/components/hullcomponent.hpp"
#include "entity/components/shipdetailscomponent.hpp"
#include "entity/entity.hpp"
#include "fleet/fleet.hpp"

#include <genesis.h>
#include <imgui/imgui_impl.h>

#include <sstream>

namespace Hyperscape
{

BattleController::BattleController( const FleetSharedPtr& pPlayerFleet, const FleetSharedPtr& pOtherFleet )
    : m_DebugUIOpen( false )
    , m_pPlayerFleet( pPlayerFleet )
    , m_pOtherFleet( pOtherFleet )
{
    m_DebugUITitleColor = ImColor( 86, 224, 199, 255 ).Value;

    Genesis::ImGuiImpl::RegisterDevMenu( "Game", "Battle controller", &m_DebugUIOpen );
}

BattleController::~BattleController()
{
    Genesis::ImGuiImpl::UnregisterDevMenu( "Game", "Battle controller" );
}

void BattleController::Update( float delta )
{
    UpdateDebugUI();
}

void BattleController::Render( const Genesis::SceneCameraSharedPtr& pCamera )
{
}

void BattleController::AddBattleEvent( const BattleEventUniquePtr& pBattleEvent )
{
}

void BattleController::UpdateDebugUI()
{
    if ( Genesis::ImGuiImpl::IsEnabled() && m_DebugUIOpen )
    {
        //ImGui::SetNextWindowSize( ImVec2( 800.0f, 400.0f ) );
        if ( ImGui::Begin( "Battle controller", &m_DebugUIOpen ) )
        {
            UpdateFleetDebugUI( m_pPlayerFleet, "Player's fleet" );
            ImGui::SameLine();
            UpdateFleetDebugUI( m_pOtherFleet, "Other fleet" );
            ImGui::SameLine();
            UpdateBattleEventsDebugUI();

            ImGui::End();
        }
    }
}

void BattleController::UpdateFleetDebugUI( FleetSharedPtr& pFleet, const std::string& title )
{
    if ( ImGui::BeginChild( title.c_str(), ImVec2( 400.0f, 0.0f ), true ) )
    {
        ImGui::TextColored( m_DebugUITitleColor, "Ships - %s", title.c_str() );
        ImGui::Separator();

        int shipIndex = 1;
        for ( auto& pShip : pFleet->GetShips() )
        {
            ShipDetailsComponent* pShipDetailsComponent = pShip->GetComponent<ShipDetailsComponent>();
            SDL_assert( pShipDetailsComponent );
            if ( pShipDetailsComponent == nullptr )
            {
                continue;
            }

            std::stringstream shipTitle;
            shipTitle << "Ship #" << shipIndex++ << ": " << pShipDetailsComponent->GetShipName();
            if ( ImGui::CollapsingHeader( shipTitle.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                HullComponent* pHullComponent = pShip->GetComponent<HullComponent>();
                if ( pHullComponent != nullptr )
                {
                    ImGui::TextColored( m_DebugUITitleColor, "Hull" );
                    ImGui::Text( "- Type: %s", magic_enum::enum_name( pHullComponent->GetHullType() ).data() );
                    ImGui::Text( "- Hit points: %d / %d", pHullComponent->GetCurrentHitPoints(), pHullComponent->GetMaximumHitPoints() );
                }
            }
        }

        ImGui::EndChild();
    }
}

void BattleController::UpdateBattleEventsDebugUI()
{
    if ( ImGui::BeginChild( "Events", ImVec2( 0.0f, 0.0f ), true ) )
    {
        ImGui::TextColored( m_DebugUITitleColor, "%s", "Events" );
        ImGui::Separator();
        ImGui::EndChild();
    }
}

} // namespace Hyperscape
