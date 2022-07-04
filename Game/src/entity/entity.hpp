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
    virtual ~Entity() override {}

    virtual void Update(float delta) override;
    virtual void Render() override;

    void AddComponent(ComponentUniquePtr pComponent);
    template <typename T> T* GetComponent() { return nullptr; }
    std::vector<Component*> GetComponents();

private:
    std::array<std::vector<ComponentUniquePtr>, static_cast<size_t>(ComponentType::Count)> m_Components;
};

} // namespace Hyperscape
