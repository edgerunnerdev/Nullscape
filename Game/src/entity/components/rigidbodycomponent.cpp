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

#include <imgui/imgui.h>
#include <genesis.h>
#include <physics/rigidbody.h>
#include <physics/shape.h>
#include <physics/simulation.h>

#include "hexterminate.h"

#include "entity/components/rigidbodycomponent.hpp"

namespace Nullscape
{

RigidBodyComponent::RigidBodyComponent()
    : m_Version(1)
    , m_CentreOfMass(0.0f)
    , m_Mass(1)
{
}

RigidBodyComponent::~RigidBodyComponent() 
{

}

void RigidBodyComponent::Initialize() 
{
    using namespace Genesis::Physics;

    glm::vec3 startPosition;

    // The ship has a single compound shape which will contain shapes for each individual module.
    m_pCompoundShape = std::make_shared<CompoundShape>();

    // Create the actual physics shapes, as they have to be offset by the centre of mass.
    //for (int x = x1; x <= x2; ++x)
    //{
    //    for (int y = y1; y <= y2; ++y)
    //    {
    //        Module* pModule = m_ModuleHexGrid.Get(x, y);
    //        if (pModule != nullptr)
    //        {
    //            CylinderShapeSharedPtr pShape = std::make_shared<CylinderShape>(CylinderShapeAxis::Z, sModuleWidth, sModuleWidth, 40.0f);
    //            pShape->SetUserData(pModule->GetCollisionInfo());

    //            glm::vec3 modulePos = pModule->GetLocalPosition();
    //            m_pCompoundShape->AddChildShape(pShape, glm::translate(modulePos - m_CentreOfMass));
    //            pModule->SetPhysicsShape(pShape);

    //            m_Modules.push_back(pModule);
    //        }
    //    }
    //}

    RigidBodyConstructionInfo ci;
    ci.SetShape(m_pCompoundShape);
    ci.SetWorldTransform(glm::translate(startPosition));
    ci.SetLinearDamping(1.25f);
    ci.SetAngularDamping(2.0f);
    ci.SetMass(m_Mass);
    ci.SetCentreOfMass(m_CentreOfMass);
    m_pRigidBody = std::make_unique<RigidBody>(ci);

    g_pGame->GetPhysicsSimulation()->Add(m_pRigidBody.get());

    // The ship can only move in the XY plane and can only rotate around the Z axis.
    m_pRigidBody->SetLinearFactor(glm::vec3(1.0f, 1.0f, 0.0f));
    m_pRigidBody->SetAngularFactor(glm::vec3(0.0f, 0.0f, 1.0f));
}

void RigidBodyComponent::UpdateDebugUI() 
{

}

} // namespace Nullscape
