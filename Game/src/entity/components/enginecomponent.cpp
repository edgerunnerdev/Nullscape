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

namespace Nullscape
{

EngineComponent::EngineComponent()
    : m_AlignmentTime(1.0f)
    , m_CurrentSpeed(0.0f)
    , m_MaximumSpeed(10.0f)
    , m_Acceleration(0.25f)
    , m_TargetThrottle(0.0f)
{

}

EngineComponent::~EngineComponent() 
{

}

void EngineComponent::Update(float delta) 
{

}

void EngineComponent::UpdateDebugUI() 
{
    ImGui::SliderFloat("Alignment time", &m_AlignmentTime, 1.0f, 20.0f);
    ImGui::SliderFloat("Maximum speed", &m_MaximumSpeed, 0.1f, 20.0f);
}

void EngineComponent::Render()
{

}

bool EngineComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    Component::Serialize(data, "alignment_time", m_AlignmentTime);
    Component::Serialize(data, "current_speed", m_CurrentSpeed);
    Component::Serialize(data, "maximum_speed", m_MaximumSpeed);
    Component::Serialize(data, "acceleration", m_Acceleration);
    Component::Serialize(data, "target_throttle", m_TargetThrottle);
    return success;
}

bool EngineComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    success &= TryDeserialize(data, "alignment_time", m_AlignmentTime);
    success &= TryDeserialize(data, "current_speed", m_CurrentSpeed);
    success &= TryDeserialize(data, "maximum_speed", m_MaximumSpeed);
    success &= TryDeserialize(data, "acceleration", m_Acceleration);
    success &= TryDeserialize(data, "target_throttle", m_TargetThrottle);
    return success;
}

void EngineComponent::CloneFrom(Component* pComponent) 
{
    Component::CloneFrom(pComponent);
    EngineComponent* pOtherComponent = reinterpret_cast<EngineComponent*>(pComponent);
    m_AlignmentTime = pOtherComponent->m_AlignmentTime;
    m_CurrentSpeed = pOtherComponent->m_CurrentSpeed;
    m_MaximumSpeed = pOtherComponent->m_MaximumSpeed;
    m_Acceleration = pOtherComponent->m_Acceleration;
    m_TargetThrottle = pOtherComponent->m_TargetThrottle;
}

} // namespace Nullscape
