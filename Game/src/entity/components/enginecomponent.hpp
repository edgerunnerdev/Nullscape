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

#pragma once

#include "entity/component.hpp"

namespace Nullscape
{

class EngineComponent : public Component
{
public:
    EngineComponent();
    virtual ~EngineComponent() override;

    virtual void Initialize() override {}
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override;
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;

    DEFINE_COMPONENT(EngineComponent);

    float GetAlignmentTime() const;
    float GetCurrentSpeed() const;
    float GetMaximumSpeed() const;
    float GetAcceleration() const;
    float GetTargetThrottle() const;
    void SetTargetThrottle(float value);

private:
    float m_AlignmentTime;
    float m_CurrentSpeed;
    float m_MaximumSpeed;
    float m_Acceleration;
    float m_TargetThrottle;
};

inline float EngineComponent::GetAlignmentTime() const 
{
    return m_AlignmentTime;
}

inline float EngineComponent::GetCurrentSpeed() const
{
    return m_CurrentSpeed;
}

inline float EngineComponent::GetMaximumSpeed() const
{
    return m_MaximumSpeed;
}

inline float EngineComponent::GetAcceleration() const
{
    return m_Acceleration;
}

inline float EngineComponent::GetTargetThrottle() const
{
    return m_TargetThrottle;
}

inline void EngineComponent::SetTargetThrottle(float value)
{
    m_TargetThrottle = value;
}

} // namespace Nullscape
