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

#include "entity/components/swaycomponent.hpp"

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"

#include <glm/gtx/transform.hpp>

#include <genesis.h>
#include <imgui/imgui.h>
#include <math/misc.h>

namespace Hyperscape
{

SwayComponent::SwayComponent()
    : m_Version(1)
    , m_BaseTransformSet(false)
    , m_BaseTransform(1.0f)
    , m_Amplitude(1.0f)
{
    m_Values[0] = gRand(0.0f, 100.0f);
    m_Values[1] = gRand(0.0f, 100.0f);
    m_Values[2] = gRand(0.0f, 100.0f);
}

void SwayComponent::UpdateDebugUI()
{

}

void SwayComponent::Update(float delta)
{
    TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    if (!m_BaseTransformSet && pTransformComponent )
    {
        m_BaseTransform = pTransformComponent->GetTransform();
        m_BaseTransformSet = true;
    }

    if ( m_BaseTransformSet && pTransformComponent )
    {
        m_Values += glm::vec3(delta * 0.81, delta * 0.73, delta * 0.66);
        glm::mat4x4 m = m_BaseTransform * glm::translate(glm::cos(m_Values));
        pTransformComponent->SetTransform(m);
    }
}

bool SwayComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    return success;
}

bool SwayComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    return success;
}

void SwayComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
}

} // namespace Hyperscape
