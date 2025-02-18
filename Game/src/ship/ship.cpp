// Copyright 2014 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#include "ship/ship.h"

#include "achievements.h"
#include "collisionmasks.h"
#include "faction/faction.h"
#include "globals.h"
#include "game.hpp"
#include "menus/intelwindow.h"
#include "menus/shiptweaks.h"
#include "misc/mathaux.h"
#include "particles/particleemitter.h"
#include "particles/particlemanager.h"
#include "player.h"
#include "sector/sector.h"
#include "sector/sectorinfo.h"
#include "ship/controller/controller.h"
#include "ship/controller/controllerassault.h"
#include "ship/controller/controllerkiter.h"
#include "ship/controller/controllerplayer.h"
#include "ship/controller/controllershipyard.h"
#include "ship/damagetracker.h"
#include "ship/destructionsequence.h"
#include "ship/hexgrid.h"
#include "ship/hyperspacecore.h"
#include "ship/hyperspacegate.h"
#include "ship/inventory.h"
#include "ship/shield.h"
#include "ship/shipinfo.h"
#include "ship/shipoutline.h"
#include "ship/shipshaderuniforms.h"
#include "ship/weapon.h"
#include "shipyard/shipyard.h"
#include "trail/trail.h"

#include <genesis.h>
#include <glm/gtc/matrix_access.hpp>
#include <inputmanager.h>
#include <log.hpp>
#include <math.h>
#include <math/constants.h>
#include <math/misc.h>
#include <physics/rigidbody.h>
#include <physics/shape.h>
#include <physics/simulation.h>
#include <render/debugrender.h>
#include <render/material.hpp>
#include <resources/resourcemodel.h>
#include <resources/resourcesound.h>
#include <scene/scene.h>
#include <shaderuniform.h>
#include <sound/soundinstance.h>
#include <sstream>

namespace Hyperscape
{

Ship::Ship()
    : m_pController(nullptr)
    , m_Thrust(ShipThrust::None)
    , m_Steer(ShipSteer::None)
    , m_Strafe(ShipStrafe::None)
    , m_Dodge(ShipDodge::None)
    , m_pRigidBody(nullptr)
    , m_Energy(0.0f)
    , m_EnergyCapacity(0.0f)
    , m_DockingState(DockingState::Undocked)
    , m_pShipyard(nullptr)
    , m_EditLock(false)
    , m_pHyperspaceCore(nullptr)
    , m_pShield(nullptr)
    , m_pDestructionSequence(nullptr)
    , m_AmountToRepair(0.0f)
    , m_RepairTimer(0.0f)
    , m_RepairStep(0.0f)
    , m_IsTerminating(false)
    , m_IsDestroyed(false)
    , m_UpdatingLinks(false)
    , m_pShipInfo(nullptr)
    , m_pUniforms(nullptr)
    , m_EngineDisruptionTimer(0.0f)
    , m_RammingSpeedTimer(0.0f)
    , m_RammingSpeedCooldown(0.0f)
    , m_pDamageTracker(nullptr)
    , m_CentreOfMass(0.0f)
    , m_CollisionCallbackHandle(Genesis::Physics::InvalidCollisionCallbackHandle)
{
    m_DodgeTimer = 0.0f;
    m_TowerPosition = glm::vec3(0.0f);
    m_BoundingBoxTopLeft = glm::vec3(0.0f);
    m_BoundingBoxBottomRight = glm::vec3(0.0f);
}

Ship::~Ship()
{
    int x1, y1, x2, y2;
    m_ModuleHexGrid.GetBoundingBox(x1, y1, x2, y2);
    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            Module* pModule = m_ModuleHexGrid.Get(x, y);
            if (pModule != nullptr)
            {
                delete pModule;
            }
        }
    }

    DestroyRigidBody();

    delete m_pHyperspaceCore;
    m_pHyperspaceCore = nullptr;

    delete m_pShield;
    m_pShield = nullptr;

    delete m_pDestructionSequence;
    m_pDestructionSequence = nullptr;

    delete m_pController;
    m_pController = nullptr;

    delete m_pDamageTracker;
    m_pDamageTracker = nullptr;

    if (m_pEngineSound != nullptr && m_pEngineSound->IsPlaying())
    {
        m_pEngineSound->Stop();
        m_pEngineSound = nullptr;
    }

    if (m_pRammingSpeedSound != nullptr && m_pRammingSpeedSound->IsPlaying())
    {
        m_pRammingSpeedSound->Stop();
        m_pRammingSpeedSound = nullptr;
    }

    delete m_pUniforms;

    if (m_CollisionCallbackHandle != Genesis::Physics::InvalidCollisionCallbackHandle && g_pGame != nullptr)
    {
        g_pGame->GetPhysicsSimulation()->UnregisterCollisionCallback(m_CollisionCallbackHandle);
    }
}

void Ship::ModuleEditLock()
{
    SDL_assert(!m_EditLock);
    m_EditLock = true;
    delete m_pShield;
    m_pShield = nullptr;
}

void Ship::ModuleEditUnlock()
{
    SDL_assert(m_EditLock);
    m_EditLock = false;

    if (m_ShieldModules.empty() == false)
    {
        m_pShield = new Shield(this);
    }

    CreateRigidBody();

    // Forces recalculation of the gate's bounding box, so it matches with this ship's new shape
    if (m_pHyperspaceCore != nullptr && m_pHyperspaceCore->GetHyperspaceGate() != nullptr)
    {
        m_pHyperspaceCore->GetHyperspaceGate()->Initialise();
    }

    for (auto& pModule : m_Modules)
    {
        pModule->OnAllModulesCreated();
    }
}

Module* Ship::AddModule(ModuleInfo* pModuleInfo, int x, int y)
{
    SDL_assert(m_EditLock);

    if (pModuleInfo != nullptr)
    {
        SDL_assert(m_ModuleHexGrid.Get(x, y) == nullptr); // Don't try to add a module to a slot that's already being used

        Module* pModule = pModuleInfo->CreateModule();
        if (pModule != nullptr)
        {
            m_ModuleHexGrid.Set(x, y, pModule);
            pModule->SetHexGridSlot(x, y);
            pModule->SetOwner(this);
            pModule->Initialise();

            ModuleType type = pModule->GetModuleInfo()->GetType();
            if (type == ModuleType::Engine)
            {
                m_Engines.push_back(static_cast<EngineModule*>(pModule));
            }
            else if (type == ModuleType::Weapon)
            {
                WeaponModule* pWeaponModule = static_cast<WeaponModule*>(pModule);
                pWeaponModule->Initialise(this);
                m_WeaponModules.push_back(pWeaponModule);
            }
            else if (type == ModuleType::Reactor)
            {
                m_Reactors.push_back(static_cast<ReactorModule*>(pModule));
            }
            else if (type == ModuleType::Tower)
            {
                m_Towers.push_back(static_cast<TowerModule*>(pModule));
            }
            else if (type == ModuleType::Addon)
            {
                AddonModule* pAddonModule = static_cast<AddonModule*>(pModule);

                m_Addons.push_back(pAddonModule);

                if (pAddonModule->GetAddon()->GetInfo()->GetCategory() == AddonCategory::QuantumStateAlternator)
                {
                    m_Alternators.push_back(static_cast<AddonQuantumStateAlternator*>(pAddonModule->GetAddon()));
                }
            }
            else if (type == ModuleType::Shield)
            {
                m_ShieldModules.push_back(static_cast<ShieldModule*>(pModule));
            }

            return pModule;
        }
    }

    return nullptr;
}

ModuleInfo* Ship::RemoveModule(int x, int y)
{
    assert(m_EditLock);

    Module* pModule = m_ModuleHexGrid.Get(x, y);
    if (pModule != nullptr)
    {
        ModuleInfo* pModuleInfo = pModule->GetModuleInfo();
        ModuleType type = pModule->GetModuleInfo()->GetType();

        if (type == ModuleType::Engine)
        {
            m_Engines.remove(static_cast<EngineModule*>(pModule));
        }
        else if (type == ModuleType::Weapon)
        {
            m_WeaponModules.remove(static_cast<WeaponModule*>(pModule));
        }
        else if (type == ModuleType::Reactor)
        {
            m_Reactors.remove(static_cast<ReactorModule*>(pModule));
        }
        else if (type == ModuleType::Addon)
        {
            AddonModule* pAddonModule = static_cast<AddonModule*>(pModule);

            m_Addons.remove(static_cast<AddonModule*>(pModule));

            if (pAddonModule->GetAddon()->GetInfo()->GetCategory() == AddonCategory::QuantumStateAlternator)
            {
                m_Alternators.remove(static_cast<AddonQuantumStateAlternator*>(pAddonModule->GetAddon()));
            }
        }
        else if (type == ModuleType::Shield)
        {
            m_ShieldModules.remove(static_cast<ShieldModule*>(pModule));
        }
        else if (type == ModuleType::Tower)
        {
            m_Towers.remove(static_cast<TowerModule*>(pModule));
        }

        delete pModule;
        m_ModuleHexGrid.Set(x, y, nullptr);
        return pModuleInfo;
    }
    else
    {
        return nullptr;
    }
}

void Ship::Initialise()
{
    using namespace Genesis;

    m_pUniforms = new ShipShaderUniforms();
    m_pDamageTracker = new DamageTracker(this);

    CreateController();

    // So when the ship appears, it goes through the "coming out of hyperspace" sequence
    if (m_pHyperspaceCore != nullptr)
    {
        m_pHyperspaceCore->ExitHyperspace();
    }

    Genesis::ResourceSound* pEngineSoundResource = (ResourceSound*)FrameWork::GetResourceManager()->GetResource("data/sfx/engine.mp3");
    pEngineSoundResource->Initialise(SOUND_FLAG_FX | SOUND_FLAG_LOOPING | SOUND_FLAG_3D);
    m_pEngineSound = FrameWork::GetSoundManager()->CreateSoundInstance(pEngineSoundResource);
    if (m_pEngineSound != nullptr)
    {
        m_pEngineSound->SetMinimumDistance(50.0f);
    }

    using namespace std::placeholders;
    auto collisionCallbackFn = std::bind(&Ship::OnCollision, this, _1, _2, _3, _4, _5);
    m_CollisionCallbackHandle = g_pGame->GetPhysicsSimulation()->RegisterCollisionCallback(collisionCallbackFn);
}

// Creates either a ControllerPlayer or an AI controller. The AI controller depends on the weapons the ship uses.
// NOTE: Docking with a shipyard will delete the current controller and create a ControllerShipyard, which remains
// in place until the ship undocks. Then CreateController() is called again.
void Ship::CreateController()
{
    if (m_pController != nullptr)
    {
        delete m_pController;
        m_pController = nullptr;
    }

    if (g_pGame->GetPlayer() && g_pGame->GetPlayer()->GetShip() == this)
    {
        m_pController = new ControllerPlayer(this);
    }
    else
    {
        // If the ship has fixed weapons then we want the assault controller, which makes the ship take a linear path to its target.
        // The kiter controller only makes sense for turrets, otherwise it is impossible to line up a shot.
        bool hasFixedWeapons = false;

        for (auto& pWeaponModule : m_WeaponModules)
        {
            if (pWeaponModule->GetWeapon()->GetInfo()->GetBehaviour() == WeaponBehaviour::Fixed)
            {
                hasFixedWeapons = true;
                break;
            }
        }

        // If a ship has particle accelerators we'll want the fixed controller as well, otherwise the PA won't hit anything.
        if (hasFixedWeapons == false)
        {
            for (auto& pAddonModule : m_Addons)
            {
                AddonInfo* pAddonInfo = static_cast<AddonInfo*>(pAddonModule->GetModuleInfo());
                if (pAddonInfo->GetCategory() == AddonCategory::ParticleAccelerator)
                {
                    hasFixedWeapons = true;
                    break;
                }
            }
        }

        bool hasRammingProw = false;
        if (hasFixedWeapons == false) // No need to do this check if we would already be using a ControllerAssault anyway.
        {
            for (auto& pModule : m_Modules)
            {
                if (pModule->GetModuleInfo()->GetType() == ModuleType::Armour)
                {
                    ArmourInfo* pArmourInfo = static_cast<ArmourInfo*>(pModule->GetModuleInfo());
                    if (pArmourInfo->IsRammingProw())
                    {
                        hasRammingProw = true;
                        break;
                    }
                }
            }
        }

        if (hasFixedWeapons || hasRammingProw)
        {
            m_pController = new ControllerAssault(this);
        }
        else
        {
            m_pController = new ControllerKiter(this);
        }
    }
}

void Ship::CreateRigidBody()
{
    using namespace Genesis::Physics;

    glm::vec3 startPosition;

    // We only want to use the spawn data if the ship didn't exist before.
    // Otherwise, we'll reuse its transform and just recreate the physics object
    if (m_pRigidBody == nullptr)
    {
        startPosition = glm::vec3(m_ShipSpawnData.m_PositionX, m_ShipSpawnData.m_PositionY, 0.0f);
    }
    else
    {
        startPosition = m_pRigidBody->GetPosition();
        DestroyRigidBody();
    }

    // The ship has a single compound shape which will contain shapes for each individual module.
    m_pCompoundShape = std::make_shared<CompoundShape>();

    // Calculate the ship's mass and centre of mass.
    m_CentreOfMass = glm::vec3(0.0f);
    float mass = 0.0f;

    int x1, y1, x2, y2;
    m_ModuleHexGrid.GetBoundingBox(x1, y1, x2, y2);
    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            Module* pModule = m_ModuleHexGrid.Get(x, y);
            if (pModule != nullptr)
            {
                // We need to calculate the centre of mass of the overall ship, using
                // the position and mass of the individual modules.
                // Armour modifies the base module weight by a multiplier value.
                float moduleMass = BaseModuleMass;
                if (pModule->GetModuleInfo()->GetType() == ModuleType::Armour)
                {
                    moduleMass *= ((ArmourInfo*)(pModule->GetModuleInfo()))->GetMassMultiplier(this);
                }

                glm::vec3 modulePos = pModule->GetLocalPosition();
                m_CentreOfMass += modulePos * moduleMass;
                mass += moduleMass;
            }
        }
    }

    // Finish calculating centre of mass
    m_CentreOfMass /= mass;

    // Create the actual physics shapes, as they have to be offset by the centre of mass.
    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            Module* pModule = m_ModuleHexGrid.Get(x, y);
            if (pModule != nullptr)
            {
                CylinderShapeSharedPtr pShape = std::make_shared<CylinderShape>(CylinderShapeAxis::Z, sModuleWidth, sModuleWidth, 40.0f);
                pShape->SetUserData(pModule->GetCollisionInfo());

                glm::vec3 modulePos = pModule->GetLocalPosition();
                m_pCompoundShape->AddChildShape(pShape, glm::translate(modulePos - m_CentreOfMass));
                pModule->SetPhysicsShape(pShape);

                m_Modules.push_back(pModule);
            }
        }
    }

    CalculateBoundingBox();

    // If we have a shield, we want an additional shape. The shield needs to know the shape index for damage handling purposes.
    if (m_pShield != nullptr)
    {
        glm::vec3 bbCentre(m_BoundingBoxTopLeft.x + (m_BoundingBoxBottomRight.x - m_BoundingBoxTopLeft.x) / 2.0f, m_BoundingBoxTopLeft.y + (m_BoundingBoxBottomRight.y - m_BoundingBoxTopLeft.y) / 2.0f,
                           0.0f);

        const float extraRadius = 20.0f; // So the shield is a bit larger than the actual ship
        const float radiusX = abs((m_BoundingBoxBottomRight.x - m_BoundingBoxTopLeft.x)) / 2.0f + extraRadius;
        const float radiusY = abs((m_BoundingBoxBottomRight.y - m_BoundingBoxTopLeft.y)) / 2.0f + extraRadius;

        m_pShield->InitialisePhysics(bbCentre, radiusX, radiusY);
    }

    RigidBodyConstructionInfo ci;
    ci.SetShape(m_pCompoundShape);
    ci.SetWorldTransform(glm::translate(startPosition));
    ci.SetLinearDamping(1.25f);
    ci.SetAngularDamping(2.0f);
    ci.SetMass(static_cast<int>(mass));
    ci.SetCentreOfMass(m_CentreOfMass);
    m_pRigidBody = new RigidBody(ci);

    g_pGame->GetPhysicsSimulation()->Add(m_pRigidBody);

    // The ship can only move in the XY plane and can only rotate around the Z axis.
    m_pRigidBody->SetLinearFactor(glm::vec3(1.0f, 1.0f, 0.0f));
    m_pRigidBody->SetAngularFactor(glm::vec3(0.0f, 0.0f, 1.0f));
}

void Ship::DestroyRigidBody()
{
    if (m_pRigidBody == nullptr)
    {
        return;
    }

    if (g_pGame != nullptr)
    {
        g_pGame->GetPhysicsSimulation()->Remove(m_pRigidBody);
        delete m_pRigidBody;
        m_pRigidBody = nullptr;
    }

    m_Modules.clear();
}

void Ship::Update(float delta)
{
    // It is still possible for the ship to be updated once after the sector is deleted
    if (g_pGame->GetCurrentSector() == nullptr)
    {
        return;
    }

    if (g_pGame->IsPaused())
    {
        return;
    }

    ShipTweaks* pShipTweaks = g_pGame->GetCurrentSector()->GetShipTweaks();
    GetRigidBody()->SetAngularDamping(pShipTweaks->GetAngularDamping());
    GetRigidBody()->SetLinearDamping(pShipTweaks->GetLinearDamping());

    // If our ship has finished docking, then it can be edited
    if (GetDockingState() == DockingState::Docked && m_EditLock == false)
    {
        ModuleEditLock();
    }

    // The tower, being the most important part of a Ship, is what actually has to be targeted by other ships or
    // navigated towards - the theoretical centre of the ship or even its centre of mass won't actually match the
    // tower's position, so we need to calculate it explicitly.
    TowerModule* pTowerModule = GetTowerModule();
    if (pTowerModule != nullptr)
    {
        // Local HexGrid coordinates to local space
        glm::vec3 moduleLocalPos = pTowerModule->GetLocalPosition();

        // Local space to world space
        m_TowerPosition = glm::vec3(m_pRigidBody->GetWorldTransform() * glm::vec4(moduleLocalPos, 1.0f));

        if (IsDestroyed() == false)
            m_pController->Update(delta);
    }

    if (m_EditLock == false)
    {
        for (auto& pModule : m_Modules)
        {
            pModule->Update(delta);
        }
    }
    else
    {
        int x1, y1, x2, y2;
        m_ModuleHexGrid.GetBoundingBox(x1, y1, x2, y2);

        for (int x = x1; x <= x2; ++x)
        {
            for (int y = y1; y <= y2; ++y)
            {
                Module* pModule = m_ModuleHexGrid.Get(x, y);
                if (pModule != nullptr)
                {
                    pModule->UpdateShipyard(delta);
                }
            }
        }
    }

    UpdateReactors(delta);
    UpdateRepair(delta);
    UpdateShield(delta);
    UpdateEngines(delta);
    UpdateSounds(delta);

    if (m_pHyperspaceCore)
    {
        m_pHyperspaceCore->Update(delta);
    }

    if (m_pDestructionSequence)
    {
        m_pDestructionSequence->Update(delta);
    }

    // Reset thrust / steer / strafe / dodge, as the controllers are responsible for setting those every frame.
    SetThrust(ShipThrust::None);
    SetSteer(ShipSteer::None);
    SetStrafe(ShipStrafe::None);
    SetDodge(ShipDodge::None);
}

void Ship::ApplyDodge(float& timer, float enginePower)
{
    float forceMultiplier = g_pGame->GetCurrentSector()->GetShipTweaks()->GetEvasionProtocolsForceMultiplier();
    glm::vec3 impulse(glm::column(m_pRigidBody->GetWorldTransform(), 0));
    impulse *= enginePower * forceMultiplier;
    m_pRigidBody->ApplyLinearImpulse(impulse);
    timer = 2.0f;
}

void Ship::ApplyStrafe(float enginePower)
{
    glm::vec3 force = glm::vec3(glm::column(m_pRigidBody->GetWorldTransform(), 0)) * enginePower;
    force *= HasPerk(Perk::ThrustVectoring) ? 0.6f : 0.4f;

    if (m_Strafe == ShipStrafe::Left)
    {
        m_pRigidBody->ApplyLinearForce(-force);
    }
    else if (m_Strafe == ShipStrafe::Right)
    {
        m_pRigidBody->ApplyLinearForce(force);
    }
}

void Ship::UpdateEngines(float delta)
{
    float enginePower = 0.0f;
    float torque = 0.0f;
    int numEngines = 0;

    if (m_EnergyCapacity > 0.0f) // For the ship to move at least one reactor must still be operational
    {
        for (auto& pEngine : m_Engines)
        {
            if (pEngine->IsDestroyed() == false && pEngine->IsEMPed() == false)
            {
                const EngineInfo* pEngineInfo = (EngineInfo*)pEngine->GetModuleInfo();
                enginePower += pEngineInfo->GetThrust();
                torque += pEngineInfo->GetTorque();
                numEngines++;

                Trail* pTrail = pEngine->GetTrail();
                if (pTrail != nullptr)
                {
                    glm::vec3 moduleLocalPos = pEngine->GetLocalPosition() + pEngine->GetTrailOffset();
                    glm::vec3 moduleWorldPos = glm::vec3(GetRigidBody()->GetWorldTransform() * glm::vec4(moduleLocalPos, 1.0f));
                    pTrail->AddPoint(moduleWorldPos);
                }
            }
        }
    }

    if (numEngines > 0)
    {
        enginePower /= numEngines;
        enginePower += enginePower * 0.2f * (static_cast<float>(numEngines) - 1.0f);

        float enginePowerMultiplier = 1.0f;
        if (IsRammingSpeedEnabled())
        {
            enginePowerMultiplier = 3.0f;
            m_RammingSpeedTimer -= delta;
        }
        else if (m_RammingSpeedCooldown > 0.0f)
        {
            m_RammingSpeedCooldown -= delta;
        }

        for (auto& pAddonModule : m_Addons)
        {
            AddonInfo* pAddonInfo = static_cast<AddonInfo*>(pAddonModule->GetModuleInfo());
            if (pAddonInfo->GetCategory() == AddonCategory::FuelInjector && pAddonModule->GetAddon()->IsActive())
            {
                AddonFuelInjector* pAddonFuelInjector = static_cast<AddonFuelInjector*>(pAddonModule->GetAddon());
                if (pAddonFuelInjector->GetEnginePowerMultiplier() > enginePowerMultiplier)
                {
                    enginePowerMultiplier = pAddonFuelInjector->GetEnginePowerMultiplier();
                }
            }
        }

        if (HasPerk(Perk::EngineRemapping))
        {
            enginePowerMultiplier *= 1.1f;
        }

        if (AreEnginesDisrupted())
        {
            torque *= 0.5f;
            enginePower *= 0.25f;
            m_EngineDisruptionTimer -= delta;
        }

        torque *= cEngineTorqueMultiplier * enginePowerMultiplier;
        enginePower *= cEngineThrustMultiplier * enginePowerMultiplier;

        ShipTweaks* pShipTweaks = g_pGame->GetCurrentSector()->GetShipTweaks();
        torque *= pShipTweaks->GetEngineTorqueMultiplier();
        enginePower *= pShipTweaks->GetEngineThrustMultiplier();

        // Turn the ship as required.
        // Forces are only applied if needed so the body can be asleep if it is in a standstill. Applying a force of 0.0f would wake it up.
        if (m_Steer == ShipSteer::Right)
        {
            m_pRigidBody->ApplyAngularForce(glm::vec3(0.0f, 0.0f, -torque * enginePowerMultiplier));
        }
        else if (m_Steer == ShipSteer::Left)
        {
            m_pRigidBody->ApplyAngularForce(glm::vec3(0.0f, 0.0f, torque * enginePowerMultiplier));
        }

        if (HasPerk(Perk::EvasionProtocols))
        {
            if (GetDodge() == ShipDodge::Left && m_DodgeTimer <= 0.0f)
            {
                ApplyDodge(m_DodgeTimer, -enginePower);
            }
            else if (GetDodge() == ShipDodge::Right && m_DodgeTimer <= 0.0f)
            {
                ApplyDodge(m_DodgeTimer, enginePower);
            }

            m_DodgeTimer = std::max(0.0f, m_DodgeTimer - delta);
        }

        // Advance the ship as required. The ship comes to a stop through its linear and angular damping, there is no explicity way of stopping it.
        // We apply the forces to the center of mass so as to prevent unwanted torque.
        glm::vec3 forwardForce = glm::vec3(glm::column(m_pRigidBody->GetWorldTransform(), 1)) * enginePower;
        if (m_Thrust == ShipThrust::Forward)
        {
            m_pRigidBody->ApplyLinearForce(forwardForce);
        }
        else if (m_Thrust == ShipThrust::Backward)
        {
            const glm::vec3 reverseForce = -forwardForce * (HasPerk(Perk::ThrustVectoring) ? 0.6f : 0.4f);
            m_pRigidBody->ApplyLinearForce(reverseForce);
        }

        ApplyStrafe(enginePower);
    }
}

void Ship::UpdateReactors(float delta)
{
    // Consider all reactors disabled while editing
    if (m_EditLock)
    {
        m_Energy = 0.0f;
        return;
    }

    m_EnergyCapacity = 0.0f;

    float rechargeRate = 0.0f;
    for (auto& pReactor : m_Reactors)
    {
        if (pReactor->IsDestroyed() == false && pReactor->IsEMPed() == false)
        {
            m_EnergyCapacity += pReactor->GetCapacity();
            rechargeRate += pReactor->GetRechargeRate();
        }
    }

    m_Energy = gClamp<float>(m_Energy + rechargeRate * delta, 0.0f, m_EnergyCapacity);
}

void Ship::UpdateRepair(float delta)
{
    if (m_AmountToRepair <= 0.0f || m_EditLock)
        return;

    float amount = m_RepairStep * delta;
    if (m_AmountToRepair - amount < 0.0f) // Prevent over-repairing
        amount = m_AmountToRepair;

    for (auto& pModule : m_Modules)
    {
        // If a module has already been destroyed, it shouldn't be affected by repairs unless
        // the ship is docked. Repairing a destroyed module does not restore the collision but
        // that isn't a problem as the whole physics object is recreated on undock.
        if (pModule->IsDestroyed() == false || GetDockingState() == DockingState::Docked || GetDockingState() == DockingState::Docking)
        {
            pModule->Repair(amount);
        }
    }

    m_AmountToRepair = gMax(0.0f, m_AmountToRepair - m_RepairStep * delta);
    m_RepairTimer = gMax(0.0f, m_RepairTimer - delta);
}

void Ship::UpdateShield(float delta)
{
    if (m_pShield != nullptr)
    {
        m_pShield->Update(delta);
    }
}

void Ship::UpdateSounds(float delta)
{
    if (GetTowerModule() == nullptr)
        return;

    const glm::vec3& position = GetTowerPosition();

    glm::vec3 velocity(0.0f);
    if (GetRigidBody() != nullptr)
    {
        velocity = GetRigidBody()->GetLinearVelocity();
    }

    if (m_pEngineSound != nullptr && m_pEngineSound->IsPlaying())
    {
        m_pEngineSound->Set3DAttributes(&position, &velocity);
    }

    if (m_pRammingSpeedSound != nullptr && m_pRammingSpeedSound->IsPlaying())
    {
        m_pRammingSpeedSound->Set3DAttributes(&position, &velocity);
    }
}

void Ship::Render()
{
    if (IsVisible() == false)
    {
        return;
    }

    glEnable(GL_DEPTH_TEST);

    glm::mat4 modelTransform = GetRigidBody()->GetWorldTransform();
    if (GetHyperspaceCore() != nullptr)
    {
        GetHyperspaceCore()->GetHyperspaceGate()->Render(modelTransform);
    }

    RenderModuleHexGrid(modelTransform);

    if (m_pShield != nullptr)
    {
        m_pShield->Render(modelTransform);
    }

    glDisable(GL_DEPTH_TEST);
}

void Ship::RenderModuleHexGrid(const glm::mat4& modelTransform)
{
    if (m_EditLock)
    {
        int x1, y1, x2, y2;
        m_ModuleHexGrid.GetBoundingBox(x1, y1, x2, y2);

        glm::vec3 moduleLocalPos;
        for (int x = x1; x <= x2; ++x)
        {
            for (int y = y1; y <= y2; ++y)
            {
                Module* pModule = m_ModuleHexGrid.Get(x, y);
                if (pModule != nullptr && pModule->ShouldRender())
                {
                    for (Genesis::MaterialSharedPtr& pMaterial : pModule->GetModel()->GetMaterials())
                    {
                        SetSharedShaderParameters(pModule, pMaterial.get());
                    }

                    moduleLocalPos = Module::GetLocalPosition(this, x, y);
                    glm::mat4 worldTransform = modelTransform * glm::translate(moduleLocalPos);
                    pModule->Render(worldTransform, false);
                }
            }
        }
    }
    else
    {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        glm::vec3 moduleLocalPos;
        for (auto& pModule : m_Modules)
        {
            if (pModule->ShouldRender())
            {
                for (Genesis::MaterialSharedPtr& pMaterial : pModule->GetModel()->GetMaterials())
                {
                    SetSharedShaderParameters(pModule, pMaterial.get());
                }

                moduleLocalPos = pModule->GetLocalPosition();
                glm::mat4 worldTransform = modelTransform * glm::translate(moduleLocalPos);
                pModule->Render(worldTransform, false);
            }
        }

        RenderModuleHexGridOutline(modelTransform);

        glDisable(GL_STENCIL_TEST);
    }
}

void Ship::RenderModuleHexGridOutline(const glm::mat4& modelTransform)
{
    ShipOutline* pShipOutline = g_pGame->GetShipOutline();
    if (pShipOutline->AffectsShip(this) == false)
    {
        return;
    }

    glm::vec3 moduleLocalPos;
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    for (auto& pModule : m_Modules)
    {
        if (pModule->ShouldRender())
        {
            moduleLocalPos = pModule->GetLocalPosition();
            glm::mat4 worldTransform = modelTransform * glm::translate(moduleLocalPos);
            pModule->Render(worldTransform, true);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void Ship::SetSharedShaderParameters(Module* pModule, Genesis::Material* pMaterial)
{
    //using namespace Genesis;

    //if (pMaterial->GetShader() == m_pUniforms->GetShader())
    //{
    //    const Colour& primaryColour = IsFlagship() ? GetFaction()->GetColour(FactionColourId::PrimaryFlagship) : GetFaction()->GetColour(FactionColourId::Primary);
    //    m_pUniforms->Set(ShipShaderUniform::PrimaryPaint, primaryColour.glm());

    //    const Colour& secondaryColour = IsFlagship() ? GetFaction()->GetColour(FactionColourId::SecondaryFlagship) : GetFaction()->GetColour(FactionColourId::Secondary);
    //    m_pUniforms->Set(ShipShaderUniform::SecondaryPaint, secondaryColour.glm());

    //    const float healthRatio = pModule->GetHealth() / pModule->GetModuleInfo()->GetHealth(this);
    //    m_pUniforms->Set(ShipShaderUniform::Health, healthRatio);

    //    const float repairEdgeAlpha = pModule->IsDestroyed() ? 0.0f : (m_RepairTimer > 0.0f ? 1.0f : 0.0f);
    //    m_pUniforms->Set(ShipShaderUniform::RepairEdgeAlpha, repairEdgeAlpha);

    //    const float repairEdgeOffset = pModule->IsDestroyed() ? 0.0f : (m_RepairTimer / RepairDuration) * 0.4f;
    //    m_pUniforms->Set(ShipShaderUniform::RepairEdgeOffset, repairEdgeOffset);

    //    const int isJumping = (m_pHyperspaceCore != nullptr && m_pHyperspaceCore->IsJumping()) ? 1 : 0;
    //    m_pUniforms->Set(ShipShaderUniform::ClipActive, isJumping);

    //    if (isJumping)
    //    {
    //        glm::vec3 hyperspaceClipPosition(m_pHyperspaceCore->GetHyperspaceGate()->GetGatePosition());
    //        m_pUniforms->Set(ShipShaderUniform::Clip, glm::vec4(hyperspaceClipPosition, 0.0f));

    //        glm::vec4 hyperspaceClipDirection = glm::column(GetRigidBody()->GetWorldTransform(), 1);
    //        m_pUniforms->Set(ShipShaderUniform::ClipForward, hyperspaceClipDirection);
    //    }

    //    const Background* pBackground = g_pGame->GetCurrentSector()->GetBackground();
    //    const glm::vec4& ambient = pBackground->GetAmbientColour();
    //    m_pUniforms->Set(ShipShaderUniform::AmbientColour, ambient);

    //    const Genesis::Colour& emissive = GetFaction()->GetColour(FactionColourId::Glow);
    //    m_pUniforms->Set(ShipShaderUniform::EmissiveColour, emissive.glm());

    //    const float assemblyPercentage = pModule->GetAssemblyPercentage();
    //    if (assemblyPercentage < 1.0f)
    //    {
    //        const float intensity = 1.0f - assemblyPercentage;
    //        glm::vec4 overlayColour(0.0f, 1.0f, 1.0f, intensity);
    //        m_pUniforms->Set(ShipShaderUniform::OverlayColour, overlayColour);
    //    }
    //    else if (pModule->GetModuleInfo()->GetType() == ModuleType::Armour)
    //    {
    //        ArmourModule* pArmourModule = (ArmourModule*)pModule;
    //        m_pUniforms->Set(ShipShaderUniform::OverlayColour, pArmourModule->GetOverlayColour());
    //    }
    //    else if (pModule->GetModuleInfo()->GetType() == ModuleType::Tower)
    //    {
    //        TowerModule* pTowerModule = (TowerModule*)pModule;
    //        m_pUniforms->Set(ShipShaderUniform::OverlayColour, pTowerModule->GetOverlayColour(this));
    //    }
    //    else
    //    {
    //        m_pUniforms->Set(ShipShaderUniform::OverlayColour, glm::vec4(0.0f));
    //    }

    //    const int empActive = pModule->IsEMPed() ? 1 : 0;
    //    m_pUniforms->Set(ShipShaderUniform::EMPActive, empActive);
    //}
}

bool Ship::ConsumeEnergy(float quantity)
{
    if (m_Energy - quantity >= 0.0f)
    {
        m_Energy -= quantity;
        return true;
    }
    return false;
}

void Ship::DamageModule(Weapon* pWeapon, Module* pModule, float delta)
{
    WeaponInfo* pInfo = pWeapon->GetInfo();
    DamageModule(pInfo->GetSystem(), pInfo->GetDamageType(), pInfo->GetDamage(), pInfo->GetBurst(), pWeapon->GetOwner(), pModule, delta);
}

void Ship::DamageModule(WeaponSystem weaponSystem, DamageType damageType, float damageAmount, int burst, Ship* pDealtBy, Module* pModule, float delta)
{
    float frameDamage, displayDamage;
    if (!DamageShared(weaponSystem, damageAmount, burst, pDealtBy->GetFaction(), delta, &displayDamage, &frameDamage))
    {
        return;
    }

    pModule->ApplyDamage(frameDamage, damageType, pDealtBy);
}

void Ship::DamageShield(Weapon* pWeapon, float delta, const glm::vec3& hitPosition)
{
    WeaponInfo* pInfo = pWeapon->GetInfo();
    DamageShield(pInfo->GetSystem(), pInfo->GetDamageType(), pInfo->GetDamage(), pInfo->GetBurst(), pWeapon->GetOwner(), delta, hitPosition);
}

void Ship::DamageShield(WeaponSystem weaponSystem, DamageType damageType, float damageAmount, int burst, Ship* pDealtBy, float delta, const glm::vec3& hitPosition)
{
    SDL_assert(m_pShield != nullptr);

    float frameDamage, displayDamage;
    if (!DamageShared(weaponSystem, damageAmount, burst, pDealtBy->GetFaction(), delta, &displayDamage, &frameDamage))
    {
        return;
    }

    const glm::vec3 shipPosition = GetRigidBody()->GetPosition();
    const glm::vec3 dir = shipPosition - hitPosition;
    const glm::vec3 shipForward(glm::column(GetRigidBody()->GetWorldTransform(), 1));
    const float angle = atan2f(dir.y, dir.x) - atan2f(shipForward.y, shipForward.x) - Genesis::kPi2;

    m_pShield->ApplyDamage(displayDamage, frameDamage, angle, weaponSystem, damageType, pDealtBy);
}

bool Ship::DamageShared(WeaponSystem weaponSystem, float baseDamage, int burst, Faction* pDealtByFaction, float delta, float* pDisplayDamage, float* pFrameDamage) const
{
    SDL_assert(burst > 0);

    // If we are in Edit mode, we must not be damaged until the lock is lifted and the physics object recreated.
    if (m_EditLock)
    {
        return false;
    }

    // Apply damage tweaks.
    ShipTweaks* pShipTweaks = g_pGame->GetCurrentSector()->GetShipTweaks();
    baseDamage *= pShipTweaks->GetGlobalDamageMultiplier();
    if (pDealtByFaction != nullptr)
    {
        FactionId id = pDealtByFaction->GetFactionId();
        if (id == FactionId::Player || id == FactionId::Empire)
        {
            baseDamage *= pShipTweaks->GetEmpireDamageMultiplier();
        }
    }

    // If a weapon does burst damage, be sure to split the weapon's overall damage by the burst amount.
    *pFrameDamage = baseDamage / burst;

    // The display damage isn't affected sure to make sure the shield hit effect looks good :)
    *pDisplayDamage = baseDamage;

    if (weaponSystem == WeaponSystem::Lance || weaponSystem == WeaponSystem::Ion)
    {
        *pFrameDamage *= delta;
    }

    m_pDamageTracker->AddDamage(pDealtByFaction->GetFactionId(), *pFrameDamage);
    return true;
}

void Ship::CalculateRammingDamage(const Ship* pRammingShip, const Ship* pRammedShip, const ModuleInfo* pRammingModuleInfo, float& damageToRammingShip, float& damageToRammedShip)
{
    bool usingRammingProw = false;
    if (pRammingModuleInfo->GetType() == ModuleType::Armour)
    {
        const ArmourInfo* pArmourInfo = static_cast<const ArmourInfo*>(pRammingModuleInfo);
        usingRammingProw = pArmourInfo->IsRammingProw();
    }

    const int rammingShipMass = pRammingShip->GetRigidBody()->GetMass();
    const int rammedShipMass = pRammedShip->GetRigidBody()->GetMass();
    float massRatio = static_cast<float>(rammingShipMass) / static_cast<float>(rammedShipMass);

    // If we're using a ramming prow, then always assume the mass ratio to be at least 1.
    // This allows small ships with ramming prows to be dangerous.
    if (usingRammingProw)
    {
        massRatio = std::max(massRatio, 1.0f);
    }

    // Ramming ship receives reduced damage, despite mass ratio.
    const float baseDamage = 1300.0f;
    damageToRammingShip = baseDamage / massRatio / 3.0f;
    damageToRammedShip = baseDamage * massRatio;

    // Reduce the damage dealt by ramming speed when AI ships ram Imperial ships.
    if (pRammedShip->GetFaction()->GetFactionId() == FactionId::Empire)
    {
        damageToRammedShip *= 0.66f;
    }

    // Reduce the damage received by the ramming ship when it is the player doing the ramming.
    if (pRammingShip->GetFaction()->GetFactionId() == FactionId::Player)
    {
        damageToRammingShip *= 0.33f;
    }

    // Greatly reduce damage to the ramming ship if using a ramming prow.
    if (usingRammingProw)
    {
        damageToRammingShip *= 0.25f;
    }
}

void Ship::OnCollision(Genesis::Physics::RigidBody* pRigidBodyA, Genesis::Physics::RigidBody* pRigidBodyB, Genesis::Physics::ShapeWeakPtr pShapeA, Genesis::Physics::ShapeWeakPtr pShapeB,
                       const glm::vec3& hitPosition)
{
    // Is this collision relevant to us?
    if (pRigidBodyA != GetRigidBody() && pRigidBodyB != GetRigidBody())
    {
        return;
    }

    // We receive no collision damage if we are in the shipyard
    if (IsModuleEditLocked())
    {
        return;
    }

    // Skip any damage or particle effects if we're mid-jump.
    if (GetHyperspaceCore() != nullptr && GetHyperspaceCore()->IsJumping())
    {
        return;
    }

    // This can be null if we've jumped out while collisions were being processed.
    if (g_pGame->GetCurrentSector() == nullptr)
    {
        return;
    }

    const Genesis::Physics::RigidBody* pOtherRigidBody = (pRigidBodyA == GetRigidBody()) ? pRigidBodyB : pRigidBodyA;
    const Genesis::Physics::ShapeWeakPtr pOtherShape = (pRigidBodyA == GetRigidBody()) ? pShapeB : pShapeA;
    const Genesis::Physics::ShapeWeakPtr pOurShape = (pRigidBodyA == GetRigidBody()) ? pShapeA : pShapeB;
    auto extractModuleFn = [](const Genesis::Physics::ShapeWeakPtr pShape) -> Module*
    {
        if (pShape.expired())
        {
            return nullptr;
        }
        else
        {
            ShipCollisionInfo* pShipCollisionInfo = reinterpret_cast<ShipCollisionInfo*>(pShape.lock()->GetUserData());
            SDL_assert(pShipCollisionInfo->GetType() == ShipCollisionType::Module);
            return pShipCollisionInfo->GetModule();
        }
    };
    Module* pOtherModule = extractModuleFn(pOtherShape);
    Module* pOurModule = extractModuleFn(pOurShape);
    if (pOtherModule == nullptr || pOtherModule->GetOwner() == nullptr || pOurModule == nullptr)
    {
        return;
    }
    Ship* pOtherShip = pOtherModule->GetOwner();

    float damageToRammedShip;
    float damageToRammingShip;

    if (pOtherShip->IsRammingSpeedEnabled() && pOtherShip->AreEnginesDisrupted() == false)
    {
        CalculateRammingDamage(pOtherShip, this, pOtherModule->GetModuleInfo(), damageToRammingShip, damageToRammedShip);
        pOurModule->ApplyDamage(damageToRammedShip, DamageType::Collision, this);
        pOtherModule->ApplyDamage(damageToRammingShip, DamageType::TrueDamage, this);
    }

    if (IsRammingSpeedEnabled() && AreEnginesDisrupted() == false)
    {
        CalculateRammingDamage(this, pOtherShip, pOurModule->GetModuleInfo(), damageToRammingShip, damageToRammedShip);
        pOtherModule->ApplyDamage(damageToRammedShip, DamageType::Collision, this);
        pOurModule->ApplyDamage(damageToRammingShip, DamageType::TrueDamage, this);
    }

    ParticleManager* pParticleManager = g_pGame->GetCurrentSector()->GetParticleManager();
    ParticleEmitter* pEmitter = pParticleManager->GetAvailableEmitter();
    pEmitter->SetBlendMode(Genesis::BlendMode::Add);
    pEmitter->SetParticleCount(5);
    pEmitter->SetEmissionDelay(0.0f);
    pEmitter->SetPosition(hitPosition);
    pEmitter->SetTextureAtlas("data/particles/Fire_Sprites_v3.png", 512, 512, 64);
    pEmitter->SetScale(0.1f, 0.2f);
    pEmitter->SetLifetime(0.25f, 0.5f);
    pEmitter->Start();
}

void Ship::Dock(Shipyard* pShipyard)
{
    m_pShipyard = pShipyard;
    m_DockingState = DockingState::Docking;

    // Repairing 4000hp is enough to repair any module in the game
    // This is kept deliberately sensible (rather than FLT_MAX) as modules are
    // repaired in ticks which are a fraction of the repair value.
    Repair(4000.0f);

    delete m_pController;
    m_pController = new ControllerShipyard(this);

    g_pGame->SetCursorType(CursorType::Pointer);

    // Prevents the trail for stretching to the new position due to the ship being
    // teleported when docking.
    for (auto& pEngine : m_Engines)
    {
        pEngine->Disable();
    }
}

void Ship::Undock()
{
    m_pShipyard = nullptr;
    m_DockingState = DockingState::Undocked;

    ModuleEditUnlock();
    UpdateModuleLinkState();

    delete m_pController;
    m_pController = nullptr;
    CreateController();

    // Forces recalculation of the gate's bounding box, so it matches with this ship's new shape
    if (m_pHyperspaceCore != nullptr && m_pHyperspaceCore->GetHyperspaceGate() != nullptr)
    {
        m_pHyperspaceCore->GetHyperspaceGate()->CalculateBoundingBox();
    }

    g_pGame->SetCursorType(CursorType::Crosshair);

    // Restore engine functionality previously disabled by Dock()
    for (auto& pEngine : m_Engines)
    {
        pEngine->Enable();
    }

    // Unlock the "Tired of repainting it" achievement if the player has a shield.
    if (GetShield())
    {
        g_pGame->GetAchievementsManager()->UnlockAchievement(ACH_TIRED_OF_REPAINTING_IT);
    }

    // Unlock the "To gorge on lesser beings" achievement once the player has more
    // than 45 modules in his ship (the threshold to be considered a dreadnaught).
    if (GetModules().size() > 45)
    {
        g_pGame->GetAchievementsManager()->UnlockAchievement(ACH_TO_GORGE_ON_LESSER_BEINGS);
    }
}

void Ship::NotifyDockingFinished()
{
    SDL_assert(m_DockingState == DockingState::Docking);
    m_DockingState = DockingState::Docked;
}

void Ship::OnShipDestroyed()
{
    if (IsDestroyed())
    {
        return;
    }

    m_IsDestroyed = true;

    PlayDestructionSequence();

    if (m_pEngineSound != nullptr && m_pEngineSound->IsPlaying())
    {
        m_pEngineSound->Stop();
        m_pEngineSound = nullptr;
    }

    if (IsFlagship())
    {
        OnFlagshipDestroyed();
    }
}

void Ship::PlayDestructionSequence()
{
    if (m_pDestructionSequence != nullptr) // Sequence already in progress, don't do anything
        return;

    m_pDestructionSequence = new DestructionSequence(this);
}

void Ship::OnFlagshipDestroyed()
{

}

void Ship::OnModuleDestroyed(Module* pModule)
{
    using namespace Genesis::Physics;
    ShapeSharedPtr pShape = GetRigidBody()->GetShape().lock();
    if (pShape)
    {
        CompoundShapeSharedPtr pCompoundShape = std::static_pointer_cast<CompoundShape>(pShape);
        if (pModule->GetPhysicsShape() != nullptr)
        {
            pCompoundShape->RemoveChildShape(pModule->GetPhysicsShape());
            pModule->SetPhysicsShape(nullptr);
        }
    }

    if (m_UpdatingLinks == false && IsDestroyed() == false)
    {
        UpdateModuleLinkState();
    }
}

void Ship::UpdateModuleLinkState()
{
    m_UpdatingLinks = true;

    for (auto& pModule : m_Modules)
    {
        pModule->SetLinked(false);
    }

    RecursiveModuleLinkState(GetTowerModule());

    for (auto& pModule : m_Modules)
    {
        if (pModule->IsLinked() == false && pModule->IsDestroyed() == false)
        {
            pModule->Destroy();
        }
    }

    m_UpdatingLinks = false;
}

void Ship::RecursiveModuleLinkState(Module* pModule)
{
    if (pModule == nullptr || pModule->IsLinked() || pModule->IsDestroyed())
    {
        return;
    }

    int slotX, slotY;
    pModule->GetHexGridSlot(slotX, slotY);
    pModule->SetLinked(true);

    RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY + 2));
    RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY - 2));

    if (slotY % 2 == 0)
    {
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX - 1, slotY + 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX - 1, slotY - 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY + 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY - 1));
    }
    else
    {
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY + 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX, slotY - 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX + 1, slotY + 1));
        RecursiveModuleLinkState(GetModuleHexGrid().SafeGet(slotX + 1, slotY - 1));
    }
}

void Ship::SpawnLoot()
{
    if (Faction::sIsEnemyOf(GetFaction(), g_pGame->GetPlayerFaction()) == false)
    {
        return;
    }

    // If the player hasn't contributed enough damage to the destruction of this ship,
    // he doesn't get rewarded.
    FactionId highestContributor;
    float highestContributorDamage;
    m_pDamageTracker->GetHighestContributor(highestContributor, highestContributorDamage);
    if (m_pDamageTracker->GetDamage(FactionId::Player) < highestContributorDamage * ShipRewardThreshold)
    {
        return;
    }

    // We might not be able to spawn anything if this faction has all rarities set to 0.
    ModuleRarity moduleRarity;
    if (GetFaction()->GetLootProbability(IsFlagship()).Get(moduleRarity) == false)
    {
        return;
    }

    const ModuleInfoVector& allModules = g_pGame->GetModuleInfoManager()->GetAllModules();
    ModuleInfoVector matchingModules;
    for (auto& pModuleInfo : allModules)
    {
        if (pModuleInfo->GetRarity() == moduleRarity)
        {
            matchingModules.push_back(pModuleInfo);
        }
    }

    if (matchingModules.empty())
    {
        Genesis::Log::Warning() << "Attempting to drop module of rarity " << (int)moduleRarity << " but none exists";
    }
    else
    {
        const int s = rand() % matchingModules.size();
        ModuleInfo* pSelectedModuleInfo = matchingModules[s];
        if (pSelectedModuleInfo->GetType() == ModuleType::Armour)
        {
            const int quantity = 3 + rand() % 2;
            g_pGame->GetPlayer()->GetInventory()->AddModule(pSelectedModuleInfo->GetName(), quantity, 0, true);
        }
        else
        {
            g_pGame->GetPlayer()->GetInventory()->AddModule(pSelectedModuleInfo->GetName(), 1, 0, true);

            // Unlock the "Favoured by the RNG gods" achievement if this module is the particle accelerator.
            if (pSelectedModuleInfo->GetName() == "LegendaryParticleAccelerator")
            {
                g_pGame->GetAchievementsManager()->UnlockAchievement(ACH_FAVOURED_BY_THE_RNG_GODS);
            }
        }
    }
}

void Ship::Repair(float repairAmount)
{
    m_AmountToRepair += repairAmount;
    m_RepairTimer = RepairDuration;
    m_RepairStep = m_AmountToRepair / RepairDuration;
}

void Ship::Terminate()
{
    m_IsTerminating = true;
}

bool Ship::IsTerminating() const
{
    return m_IsTerminating;
}

bool Ship::IsVisible() const
{
    return true;
}

void Ship::CalculateBoundingBox()
{
    SDL_assert(IsModuleEditLocked() == false);

    glm::vec3 localSpaceBoundingBox(0.0f);

    // Calculate the basic bounding box, but Module::GetLocalPosition() returns the center point of the module,
    // but we'll expand the resulting BB afterwards to take the module's size into consideration.
    bool valid = false;
    for (auto& pModule : GetModules())
    {
        glm::vec3 localSpaceModule = pModule->GetLocalPosition();
        if (!valid)
        {
            m_BoundingBoxTopLeft = localSpaceModule;
            m_BoundingBoxBottomRight = localSpaceModule;
            valid = true;
        }
        else
        {
            if (localSpaceModule.x < m_BoundingBoxTopLeft.x)
                m_BoundingBoxTopLeft.x = localSpaceModule.x;
            if (localSpaceModule.x > m_BoundingBoxBottomRight.x)
                m_BoundingBoxBottomRight.x = localSpaceModule.x;
            if (localSpaceModule.y > m_BoundingBoxTopLeft.y)
                m_BoundingBoxTopLeft.y = localSpaceModule.y;
            if (localSpaceModule.y < m_BoundingBoxBottomRight.y)
                m_BoundingBoxBottomRight.y = localSpaceModule.y;
        }
    }

    m_BoundingBoxTopLeft += glm::vec3(-sModuleHalfWidth, sModuleHalfHeight, 0.0f);
    m_BoundingBoxBottomRight += glm::vec3(sModuleHalfWidth, -sModuleHalfHeight, 0.0f);
}

void Ship::GetBoundingBox(glm::vec3& topLeft, glm::vec3& bottomRight) const
{
    topLeft = m_BoundingBoxTopLeft;
    bottomRight = m_BoundingBoxBottomRight;
}

bool Ship::HasPerk(Perk perk) const
{
    return false;
}

bool Ship::IsFlagship() const
{
    return m_pShipInfo ? m_pShipInfo->IsFlagship() : false;
}

glm::mat4x4 Ship::GetTransform() const
{
    if (m_pRigidBody == nullptr)
    {
        return glm::mat4x4(1.0f);
    }
    else
    {
        return m_pRigidBody->GetWorldTransform();
    }
}

void Ship::RammingSpeed()
{
    using namespace Genesis;

    if (m_RammingSpeedCooldown <= 0.0f)
    {
        m_RammingSpeedTimer = 4.0f;
        m_RammingSpeedCooldown = RammingSpeedCooldown;

        Genesis::ResourceSound* pEngineSoundResource = (ResourceSound*)FrameWork::GetResourceManager()->GetResource("data/sfx/ramming_speed.wav");
        pEngineSoundResource->Initialise(SOUND_FLAG_FX | SOUND_FLAG_3D);
        m_pRammingSpeedSound = FrameWork::GetSoundManager()->CreateSoundInstance(pEngineSoundResource);
        if (m_pRammingSpeedSound != nullptr)
        {
            m_pRammingSpeedSound->SetMinimumDistance(300.0f);
        }
    }
}

AddonQuantumStateAlternator* Ship::GetQuantumStateAlternator() const
{
    return m_Alternators.empty() ? nullptr : m_Alternators.front();
}

void Ship::FlipQuantumState()
{
    AddonQuantumStateAlternator* pAlternator = GetQuantumStateAlternator();
    if (pAlternator != nullptr && pAlternator->GetQuantumState() != QuantumState::Inactive)
    {
        if (pAlternator->GetQuantumState() == QuantumState::White)
        {
            pAlternator->SetState(QuantumState::Black);
        }
        else if (pAlternator->GetQuantumState() == QuantumState::Black)
        {
            pAlternator->SetState(QuantumState::White);
        }
    }
}

glm::vec3 Ship::GetCentre(TransformSpace transformSpace) const
{
    glm::vec3 bbTopLeft;
    glm::vec3 bbBottomRight;
    GetBoundingBox(bbTopLeft, bbBottomRight);
    glm::vec3 bbCentre(bbTopLeft.x + (bbBottomRight.x - bbTopLeft.x) / 2.0f, bbTopLeft.y + (bbBottomRight.y - bbTopLeft.y) / 2.0f, 0.0f);

    bbCentre -= m_CentreOfMass;

    if (transformSpace == TransformSpace::Local)
    {
        return bbCentre;
    }
    else if (transformSpace == TransformSpace::World)
    {
        glm::mat4x4 shipTransform = GetRigidBody()->GetWorldTransform();
        glm::vec3 shipCentre = glm::vec3(shipTransform * glm::vec4(bbCentre, 1.0f));
        return shipCentre;
    }
    else
    {
        SDL_assert_release(false); // transform space not implemented
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

// Returns a value between 0 and 100, representing the overall's ship health percentage.
int Ship::GetIntegrity() const
{
    if (IsDestroyed())
    {
        return 0;
    }

    const ModuleVector& modules = GetModules();
    float totalModuleIntegrity = 0.0f;
    for (const Module* pModule : modules)
    {
        totalModuleIntegrity += pModule->GetHealth() / pModule->GetModuleInfo()->GetHealth(this);
    }
    return static_cast<int>(totalModuleIntegrity / static_cast<float>(modules.size()) * 100.0f);
}

} // namespace Hyperscape