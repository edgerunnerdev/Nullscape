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

#include <array>
#include <vector>

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/vector.h>
#include <bitsery/ext/pointer.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/std_smart_ptr.h>
#include <externalheadersend.hpp>
// clang-format on

#include <scene/sceneobject.h>
#include <coredefines.h>

#include "entity/componentserialization.hpp"
#include "entity/componenttype.hpp"

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Component)

class Entity : public Genesis::SceneObject
{
public:
    Entity() {}
    virtual ~Entity() override {}

    virtual void Update(float delta) override;
    virtual void Render() override;

    void AddComponent(ComponentUniquePtr pComponent);
    template<typename T> T* GetComponent() 
    { 
        auto& components = m_Components[static_cast<size_t>(T::sType)];
        return components.empty() ? nullptr : reinterpret_cast<T*>(components.front().get());
    }
    std::vector<Component*> GetComponents();

    template<typename S>
    void serialize(S& s) 
    {
        s.container(m_Components, [](S& s, std::vector<ComponentUniquePtr>& components) 
            {
                s.container(components, 64, [](S& s, ComponentUniquePtr& pComponent) 
                {
                    s.ext(pComponent, bitsery::ext::StdSmartPtr{});
                });
            }
        );
    }


private:
    std::array<std::vector<ComponentUniquePtr>, static_cast<size_t>(ComponentType::Count)> m_Components;
};

} // namespace Hyperscape
