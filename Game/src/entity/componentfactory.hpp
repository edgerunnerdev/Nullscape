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
#include <functional>
#include <string>
#include <unordered_map>

#include <coredefines.h>

#include "entity/componenttype.hpp"

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Component)

class ComponentFactory
{
public:
    ComponentFactory();
    ~ComponentFactory();

    static ComponentFactory* Get();

    ComponentUniquePtr Create(ComponentType type) const;
    ComponentUniquePtr Create(const std::string& typeName) const;

private:
    using ConstructionFn = std::function<ComponentUniquePtr()>;
    std::array<ConstructionFn, static_cast<size_t>(ComponentType::Count)> m_TypeRegistry;
    std::unordered_map<std::string, std::function<ComponentUniquePtr()>> m_NameRegistry;
};

} // namespace Hyperscape
