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

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <array>
#include <vector>

#include <scene/sceneobject.h>
#include <coredefines.h>

#include "entity/componenttype.hpp"

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Component)

class Entity : public Genesis::SceneObject
{
public:
    Entity() {}
    Entity(const Entity& other);
    virtual ~Entity() override {}

    virtual void Update(float delta) override;
    virtual void Render() override;

    virtual void OnAddedToScene(Genesis::Scene* pScene);
    virtual void OnRemovedFromScene();

    void AddComponent(ComponentUniquePtr pComponent);
    template<typename T> T* GetComponent() 
    { 
        auto& components = m_Components[static_cast<size_t>(T::sType)];
        return components.empty() ? nullptr : reinterpret_cast<T*>(components.front().get());
    }
    std::vector<Component*> GetComponents() const;

    bool Serialize(nlohmann::json& data);
    bool Deserialize(const nlohmann::json& data);

    bool IsEditorEntity() const;
    void SetEditorEntity(bool state);

private:
    std::array<std::vector<ComponentUniquePtr>, static_cast<size_t>(ComponentType::Count)> m_Components;
    bool m_IsEditorEntity;
};

inline bool Entity::IsEditorEntity() const
{
    return m_IsEditorEntity;
}

inline void Entity::SetEditorEntity(bool state)
{
    m_IsEditorEntity = state;
}

} // namespace Hyperscape
