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

#include "entity/componentfactory.hpp"

#include "entity/components/cameracomponent.hpp"
#include "entity/components/enginecomponent.hpp"
#include "entity/components/modelcomponent.hpp"
#include "entity/components/navigationcomponent.hpp"
#include "entity/components/playercontrollercomponent.hpp"
#include "entity/components/rigidbodycomponent.hpp"
#include "entity/components/swaycomponent.hpp"
#include "entity/components/trailcomponent.hpp"
#include "entity/components/transformcomponent.hpp"

#include <memory>

namespace Hyperscape
{

std::unique_ptr<ComponentFactory> g_pComponentFactory;

#define REGISTER_COMPONENT(COMPONENT_NAME)                                              \
    {                                                                                   \
        auto createComponentFn = []() {                                                 \
            return std::make_unique<COMPONENT_NAME>();                                  \
        };                                                                              \
        m_TypeRegistry[static_cast<size_t>(COMPONENT_NAME::sType)] = createComponentFn; \
        m_NameRegistry[#COMPONENT_NAME] = createComponentFn;                            \
    }

ComponentFactory::ComponentFactory()
{
    REGISTER_COMPONENT(CameraComponent);
    REGISTER_COMPONENT(EngineComponent);
    REGISTER_COMPONENT(ModelComponent);
    REGISTER_COMPONENT(NavigationComponent);
    REGISTER_COMPONENT(PlayerControllerComponent);
    REGISTER_COMPONENT(TransformComponent);
    REGISTER_COMPONENT(RigidBodyComponent);
    REGISTER_COMPONENT(TrailComponent);
    REGISTER_COMPONENT(SwayComponent);
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
    return m_TypeRegistry[static_cast<size_t>(type)]();
}

ComponentUniquePtr ComponentFactory::Create(const std::string& typeName) const
{
    auto it = m_NameRegistry.find(typeName);
    return (it == m_NameRegistry.cend()) ? nullptr : it->second();
}

} // namespace Hyperscape
