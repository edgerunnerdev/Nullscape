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

#include "entity/components/hulltype.hpp"

#include <string>

namespace Hyperscape
{

class HullComponent : public Component
{
public:
    HullComponent();
    virtual ~HullComponent() override {}

    virtual void Initialize() override {}
    virtual void Update( float delta ) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override {}
    virtual bool Serialize( nlohmann::json& data ) override;
    virtual bool Deserialize( const nlohmann::json& data ) override;
    virtual void CloneFrom( Component* pComponent ) override;

    HullType GetHullType() const;
    void SetHullType( HullType hullType );
    const int GetMaximumHitPoints() const;
    void SetMaximumHitPoints( int value );
    const int GetCurrentHitPoints() const;
    void SetCurrentHitPoints( int value );

    DEFINE_COMPONENT( HullComponent );

private:
    uint16_t m_Version;
    HullType m_HullType;
    int m_MaximumHitPoints;
    int m_CurrentHitPoints;
};

inline HullType HullComponent::GetHullType() const { return m_HullType; }
inline void HullComponent::SetHullType( HullType hullType ) { m_HullType = hullType; }
inline const int HullComponent::GetMaximumHitPoints() const { return m_MaximumHitPoints; }
inline void HullComponent::SetMaximumHitPoints( int value ) { m_MaximumHitPoints = value; }
inline const int HullComponent::GetCurrentHitPoints() const { return m_CurrentHitPoints; }
inline void HullComponent::SetCurrentHitPoints( int value ) { m_CurrentHitPoints = value; }

} // namespace Hyperscape
