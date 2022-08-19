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
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include "entity/entity.hpp"

#include "entity/component.hpp"

namespace Nullscape
{

void Entity::AddComponent(ComponentUniquePtr pComponent) 
{
	m_Components[static_cast<size_t>(pComponent->GetType())].push_back(std::move(pComponent));
}

void Entity::Update(float delta) 
{
    for (size_t i = 0; i < m_Components.size(); ++i)
    {
        for (auto& pComponent : m_Components[i])
        {
            pComponent->Update(delta);
        }
    }
}

void Entity::Render()
{
    for (size_t i = 0; i < m_Components.size(); ++i)
    {
        for (auto& pComponent : m_Components[i])
        {
            pComponent->Render();
        }
    }
}

std::vector<Component*> Entity::GetComponents() 
{
	std::vector<Component*> components;

	for (size_t i = 0; i < m_Components.size(); ++i)
    {
		for (auto& pComponent : m_Components[i])
        {
			components.push_back(pComponent.get());
		}
    }

	return components;
}

} // namespace Nullscape
