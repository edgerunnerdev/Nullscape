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

#include "componenttype.hpp"

namespace Hyperscape
{

#define DEFINE_COMPONENT(COMPONENT_NAME) \
    static const ComponentType sType = ComponentType::COMPONENT_NAME; \
    virtual ComponentType GetType() const override { return sType; }

class Entity;

class Component
{
public:
    Component()
        : m_pEntity(nullptr)
    {
    }

    virtual ~Component() {}

    virtual ComponentType GetType() const = 0;

    virtual void Initialize() = 0;
    virtual void Update(float delta) = 0;
    virtual void UpdateDebugUI() = 0;
    virtual void Render() = 0;

    void SetOwner(Entity* pEntity);

    template <typename S> void serialize(S& s) {}

private:
    Entity* m_pEntity;
};

inline void Component::SetOwner(Entity* pEntity)
{
    m_pEntity = pEntity;
}

} // namespace Hyperscape
