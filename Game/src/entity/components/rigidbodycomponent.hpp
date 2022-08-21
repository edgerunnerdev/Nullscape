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
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <memory>

#include "entity/component.hpp"

namespace Genesis::Physics
{
class RigidBody;
using RigidBodyUniquePtr = std::unique_ptr<RigidBody>;
class CompoundShape;
using CompoundShapeSharedPtr = std::shared_ptr<CompoundShape>;
}

namespace Nullscape
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

    template <typename S> void serialize(S& s) 
    {
        s.value2b(m_Version);

        //for (int i = 0; i < 4; ++i)
        //{
        //    for (int j = 0; j < 4; ++j)
        //    {
        //        s.value4b(m_Transform[i][j]);
        //    }
        //}
    }

    DEFINE_COMPONENT(RigidBodyComponent);

private:
    uint16_t m_Version;
    Genesis::Physics::RigidBodyUniquePtr m_pRigidBody;
    Genesis::Physics::CompoundShapeSharedPtr m_pCompoundShape;
    glm::vec3 m_CentreOfMass;
    int m_Mass;
};

} // namespace Nullscape
