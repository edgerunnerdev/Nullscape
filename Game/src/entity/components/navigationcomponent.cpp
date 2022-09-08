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

#include "entity/components/navigationcomponent.hpp"

#include <sstream>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/gtx/transform.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <imgui/imgui.h>
#include <genesis.h>

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"

namespace Nullscape
{

NavigationComponent::NavigationComponent()
    : m_Version(1)
    , m_Mode(Mode::None)
    , m_SourceRotation(0.0f, 0.0f, 0.0f, 0.0f)
    , m_TargetRotation(0.0f, 0.0f, 0.0f, 0.0f)
    , m_TargetInterpolation(1.0f)
{
}

void NavigationComponent::Update(float delta)
{
    TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    if (pTransformComponent == nullptr)
    {
        return;
    }

    m_TargetInterpolation = glm::min(m_TargetInterpolation + delta, 1.0f);
    glm::mat4x4 translationTransform(glm::translate(glm::vec3(pTransformComponent->GetTransform()[3])));
    glm::mat4x4 rotationTransform(glm::slerp(m_SourceRotation, m_TargetRotation, m_TargetInterpolation));

    glm::mat4x4 newTransform = translationTransform * rotationTransform * glm::translate(glm::vec3(1.0f, 0.0f, 0.0f));
    pTransformComponent->SetTransform(newTransform);
}

bool NavigationComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    return success;
}

bool NavigationComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    return success;
}

void NavigationComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
}

void NavigationComponent::FlyTowards(const glm::vec3& direction) 
{
    m_Mode = Mode::Direction;

    TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    if (pTransformComponent == nullptr)
    {
        return;
    }

    const glm::mat4x4& currentTransform = pTransformComponent->GetTransform();

    const glm::vec3 upAbsolute(0.0f, 1.0f, 0.0f);
    const glm::vec3 right = glm::cross(direction, upAbsolute);
    const glm::vec3 up = glm::cross(right, direction);
    glm::mat4x4 directionTransform(1.0f);
    directionTransform[0] = glm::vec4(direction, 0.0f);
    directionTransform[1] = glm::vec4(up, 0.0f);
    directionTransform[2] = glm::vec4(right, 0.0f);

    m_SourceRotation = glm::quat(currentTransform);
    m_TargetRotation = glm::quat(directionTransform);
    m_TargetInterpolation = 0.0f;
}

} // namespace Nullscape
