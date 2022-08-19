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

#include "entity/componentfactory.hpp"

#include <memory>

#include "entity/components/modelcomponent.hpp"
#include "entity/components/transformcomponent.hpp"

namespace Nullscape
{

std::unique_ptr<ComponentFactory> g_pComponentFactory;

#define REGISTER_COMPONENT(COMPONENT_NAME) m_Registry[static_cast<size_t>(COMPONENT_NAME::sType)] = [](){ return std::make_unique<COMPONENT_NAME>(); }

ComponentFactory::ComponentFactory() 
{
    REGISTER_COMPONENT(ModelComponent);
    REGISTER_COMPONENT(TransformComponent);
}

ComponentFactory::~ComponentFactory() {}

ComponentFactory* ComponentFactory::Get() 
{
	if (g_pComponentFactory == nullptr)
    {
        g_pComponentFactory = std::make_unique<ComponentFactory>();
    }

    return g_pComponentFactory.get();
}

ComponentUniquePtr ComponentFactory::Create(ComponentType type) const 
{
    return m_Registry[static_cast<size_t>(type)]();
}

} // namespace Nullscape
