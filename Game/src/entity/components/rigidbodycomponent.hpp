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
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "entity/component.hpp"

#include <memory>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec3.hpp>
#include <externalheadersend.hpp>
// clang-format on

namespace Genesis::Physics
{
class RigidBody;
using RigidBodyUniquePtr = std::unique_ptr<RigidBody>;
class CompoundShape;
using CompoundShapeSharedPtr = std::shared_ptr<CompoundShape>;
} // namespace Genesis::Physics

namespace Hyperscape
{

class RigidBodyComponent : public Component
{
public:
    RigidBodyComponent();
    virtual ~RigidBodyComponent() override;

    virtual void Initialize() override;
    virtual void Update(float delta) override {}
    virtual void UpdateDebugUI() override;
    virtual void Render() override {}
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;

    DEFINE_COMPONENT(RigidBodyComponent);

private:
    uint16_t m_Version;
    Genesis::Physics::RigidBodyUniquePtr m_pRigidBody;
    Genesis::Physics::CompoundShapeSharedPtr m_pCompoundShape;
    glm::vec3 m_CentreOfMass;
    int m_Mass;
};

} // namespace Hyperscape
