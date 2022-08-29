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

#include <imgui/imgui.h>
#include <genesis.h>

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"

namespace Nullscape
{

NavigationComponent::NavigationComponent()
    : m_Version(1)
{
}

void NavigationComponent::Update(float delta)
{
    //TransformComponent* pTransformComponent = GetOwner()->GetComponent<TransformComponent>();
    //if (pTransformComponent)
    //{
    //    static float xOffset = 10.0f;
    //    glm::mat4x4 transform = pTransformComponent->GetTransform();
    //    transform[3].x += xOffset * delta;
    //    pTransformComponent->SetTransform(transform);
    //}
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

} // namespace Nullscape
