// Copyright 2022 Pedro Nunes
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
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include "entity/entity.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <magic_enum.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <genesis.h>
#include <log.hpp>
#include <render/viewport.hpp>
#include <rendersystem.h>
#include <scene/scene.h>

#include "entity/component.hpp"
#include "entity/componentfactory.hpp"

namespace Hyperscape
{

Entity::Entity(const Entity& other) 
    : m_IsEditorEntity(false)
{
    for (Component* pOtherComponent : other.GetComponents())
    {
        ComponentUniquePtr pComponent = ComponentFactory::Get()->Create(pOtherComponent->GetType());
        pComponent->CloneFrom(pOtherComponent);
        AddComponent(std::move(pComponent));
    }

    for (Component* pComponent : GetComponents())
    {
        pComponent->Initialize();
    }
}

void Entity::AddComponent(ComponentUniquePtr pComponent) 
{
    pComponent->SetOwner(this);
	m_Components[static_cast<size_t>(pComponent->GetType())].push_back(std::move(pComponent));
}

void Entity::Update(float delta) 
{
    for (size_t i = 0; i < m_Components.size(); ++i)
    {
        for (auto& pComponent : m_Components[i])
        {
            if (IsEditorEntity() && pComponent->UpdatesInEditor() == false)
            {
                continue;
            }

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

void Entity::OnAddedToScene(Genesis::Scene* pScene)
{
    for (auto& components : m_Components)
    {
        for (auto& pComponent : components)
        {
            pComponent->OnAddedToScene(pScene);
        }
    }
}

void Entity::OnRemovedFromScene() 
{
    for (auto& components : m_Components)
    {
        for (auto& pComponent : components)
        {
            pComponent->OnRemovedFromScene();
        }
    }
}

std::vector<Component*> Entity::GetComponents() const
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

bool Entity::Serialize(nlohmann::json& data) 
{
    using namespace nlohmann;
    json jComponents = json::array();
    bool success = true;
    for (auto& typeComponents : m_Components)
    {
        if (!typeComponents.empty())
        {
            json jTypeComponents = json::object();   
            for (auto& pComponent : typeComponents)
            {
                json jComponent = json::object();
                success &= pComponent->Serialize(jComponent);
                jComponents.push_back(jComponent);
            }
        }
    }
    data["components"] = jComponents;
    return success;
}

bool Entity::Deserialize(const nlohmann::json& jData) 
{
    using namespace nlohmann;
    using namespace Genesis;

    json::const_iterator componentsIt = jData.find("components");
    if (componentsIt != jData.cend())
    {
        for (const json& jComponent : componentsIt.value())
        {
            json::const_iterator typeIt = jComponent.find("type");
            if (typeIt != jComponent.cend())
            {
                const std::string& typeName = typeIt.value().get<std::string>();
                ComponentUniquePtr pComponent = std::move(ComponentFactory::Get()->Create(typeName));
                if (pComponent == nullptr)
                {
                    Log::Error() << "Failed to create component of type '" << typeName << "'.";
                    return false;
                }
                else if (!pComponent->Deserialize(jComponent))
                {
                    Log::Error() << "Failed to deserialize component of type '" << typeName << "'.";
                    return false;
                }
                else
                {
                    AddComponent(std::move(pComponent));
                }
            }
            else
            {
                Log::Error() << "Failed to create component, no 'type' specified.";
                return false;
            }
        }
    }

    for (Component* pComponent : GetComponents())
    {
        pComponent->Initialize();
    }

    return true;
}

} // namespace Hyperscape
