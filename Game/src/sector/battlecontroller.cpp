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

#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

namespace Hyperscape
{

BattleController::BattleController( const FleetSharedPtr& pPlayerFleet, const FleetSharedPtr& pOtherFleet )
    : m_DebugUIOpen( false )
{
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
        ImGui::TextColored( ImVec4( 1.0f, 0.65f, 0.0f, 1.0f ), "Ships - %s", title.c_str() );
        ImGui::EndChild();
    }
}

void BattleController::UpdateBattleEventsDebugUI()
{
    if ( ImGui::BeginChild( "Events", ImVec2( 0.0f, 0.0f ), true ) )
    {
        ImGui::TextColored( ImVec4( 1.0f, 0.65f, 0.0f, 1.0f ), "%s", "Events" );
        ImGui::EndChild();
    }
}

} // namespace Hyperscape
