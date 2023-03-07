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

#include "entity/components/reactorcomponent.hpp"

#include "entity/entity.hpp"

#include <genesis.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Hyperscape
{

ReactorComponent::ReactorComponent()
    : m_Version( 1 )
    , m_MaximumHitPoints( 100 )
    , m_CurrentHitPoints( 100 )
    , m_MaximumEnergy( 1 )
    , m_CurrentEnergy( 1.0f )
    , m_BaselineEnergyRecoveryRate( 0.0f )
    , m_EnergyRecoveryRate( 0.0f )
{
}

void ReactorComponent::UpdateDebugUI()
{
    ImGui::InputInt( "Maximum hit points", &m_MaximumHitPoints );
    ImGui::InputInt( "Current hit points", &m_CurrentHitPoints );
    ImGui::InputInt( "Maximum energy", &m_MaximumEnergy );
    ImGui::InputFloat( "Current energy", &m_CurrentEnergy );
    ImGui::InputFloat( "Baseline energy recovery rate", &m_BaselineEnergyRecoveryRate );
}

void ReactorComponent::Update( float delta )
{
    CalculateEnergyRecoveryRate();
    m_CurrentEnergy = std::min( m_CurrentEnergy + GetEnergyRecoveryRate() * delta, static_cast<float>( m_MaximumEnergy ) );
}

bool ReactorComponent::Serialize( nlohmann::json& data )
{
    bool success = Component::Serialize( data );
    data[ "maximum_hit_points" ] = m_MaximumHitPoints;
    data[ "current_hit_points" ] = m_CurrentHitPoints;
    data[ "maximum_energy" ] = m_MaximumEnergy;
    data[ "baseline_energy_recovery_rate" ] = m_BaselineEnergyRecoveryRate;
    return success;
}

bool ReactorComponent::Deserialize( const nlohmann::json& data )
{
    bool success = Component::Deserialize( data );
    success &= TryDeserialize( data, "maximum_hit_points", m_MaximumHitPoints );
    success &= TryDeserialize( data, "current_hit_points", m_CurrentHitPoints );
    success &= TryDeserialize( data, "maximum_energy", m_MaximumEnergy );
    success &= TryDeserialize( data, "baseline_energy_recovery_rate", m_BaselineEnergyRecoveryRate );

    // Intentionally not serialized as they only matter during individual fights.
    m_CurrentEnergy = m_MaximumEnergy;
    m_BaselineEnergyRecoveryRate = m_BaselineEnergyRecoveryRate;

    return success;
}

void ReactorComponent::CloneFrom( Component* pComponent )
{
    Component::CloneFrom( pComponent );
    ReactorComponent* pReactorComponent = reinterpret_cast<ReactorComponent*>( pComponent );
    m_MaximumHitPoints = pReactorComponent->m_MaximumHitPoints;
    m_CurrentHitPoints = pReactorComponent->m_CurrentHitPoints;
    m_MaximumEnergy = pReactorComponent->m_MaximumEnergy;
    m_CurrentEnergy = pReactorComponent->m_CurrentEnergy;
    m_BaselineEnergyRecoveryRate = pReactorComponent->m_BaselineEnergyRecoveryRate;
}

void ReactorComponent::SetMaximumHitPoints( int value )
{
    SDL_assert( value > 0 );
    m_MaximumHitPoints = value;
    m_CurrentHitPoints = std::min( m_CurrentHitPoints, m_MaximumHitPoints );
}

void ReactorComponent::SetCurrentHitPoints( int value )
{
    SDL_assert( value >= 0 );
    m_CurrentHitPoints = value;
}

void ReactorComponent::SetMaximumEnergy( int value )
{
    SDL_assert( value > 0 );
    m_MaximumEnergy = value;
    m_CurrentEnergy = std::min( m_CurrentEnergy, static_cast<float>( m_MaximumEnergy ) );
}

void ReactorComponent::SetCurrentEnergy( float value )
{
    SDL_assert( value >= 0.0f );
    m_CurrentEnergy = value;
}

void ReactorComponent::SetBaselineEnergyRecoveryRate( float value )
{
    SDL_assert( value >= 0.0f );
    m_BaselineEnergyRecoveryRate = value;
}

void ReactorComponent::SetEnergyRecoveryRate( float value )
{
    SDL_assert( value >= 0.0f );
    m_EnergyRecoveryRate = value;
}

void ReactorComponent::CalculateEnergyRecoveryRate()
{
    // TODO: Take into account modules which affect energy.
    // TODO: Recovery rate should be modified by reactor's health.
    m_EnergyRecoveryRate = m_BaselineEnergyRecoveryRate;
}

} // namespace Hyperscape
