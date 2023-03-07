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

#pragma once

#include "entity/component.hpp"

#include <string>

namespace Hyperscape
{

// The ReactorComponent represents a ship's primary reactor.
// Each ship must have a ReactorComponent, and it is through it which all energy management is handled.
class ReactorComponent : public Component
{
public:
    ReactorComponent();
    virtual ~ReactorComponent() override {}

    virtual void Initialize() override {}
    virtual void Update( float delta ) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override {}
    virtual bool Serialize( nlohmann::json& data ) override;
    virtual bool Deserialize( const nlohmann::json& data ) override;
    virtual void CloneFrom( Component* pComponent ) override;

    int GetMaximumHitPoints() const;
    void SetMaximumHitPoints( int value );
    int GetCurrentHitPoints() const;
    void SetCurrentHitPoints( int value );
    int GetMaximumEnergy() const;
    void SetMaximumEnergy( int value );
    float GetCurrentEnergy() const;
    void SetCurrentEnergy( float value );
    float GetBaselineEnergyRecoveryRate() const; // Energy recovery rate without any modifiers.
    void SetBaselineEnergyRecoveryRate( float value );
    float GetEnergyRecoveryRate() const; // Energy recovery rate after modified by modules, etc.

    DEFINE_COMPONENT( ReactorComponent );

private:
    void CalculateEnergyRecoveryRate();
    void SetEnergyRecoveryRate( float value );

    uint16_t m_Version;
    int m_MaximumHitPoints;
    int m_CurrentHitPoints;
    int m_MaximumEnergy;
    float m_CurrentEnergy;
    float m_BaselineEnergyRecoveryRate;
    float m_EnergyRecoveryRate;
};

inline int ReactorComponent::GetMaximumHitPoints() const { return m_MaximumHitPoints; }
inline int ReactorComponent::GetCurrentHitPoints() const { return m_CurrentHitPoints; }
inline int ReactorComponent::GetMaximumEnergy() const { return m_MaximumEnergy; }
inline float ReactorComponent::GetCurrentEnergy() const { return m_CurrentEnergy; }
inline float ReactorComponent::GetBaselineEnergyRecoveryRate() const { return m_BaselineEnergyRecoveryRate; }
inline float ReactorComponent::GetEnergyRecoveryRate() const { return m_EnergyRecoveryRate; }

} // namespace Hyperscape
