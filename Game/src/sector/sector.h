// Copyright 2015 Pedro Nunes
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

#pragma once

#include "faction/faction.h"
#include "ship/moduleinfo.h"
#include "ship/ship.fwd.h"

#include <render/debugrender.h>
#include <render/viewport.hpp>
#include <rendersystem.fwd.h>
#include <scene/layer.h>
#include <scene/scenecamera.h>

#include <list>
#include <memory>
#include <vector>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Entity);
GENESIS_DECLARE_SMART_PTR(Fleet);

namespace UI2
{
GENESIS_DECLARE_SMART_PTR(Window);
}

class AmmoManager;
class MuzzleflashManager;
class MuzzleflashManagerRep;
class LaserManager;
class SpriteManager;
class Dust;
class ShipInfo;
class Faction;
class SectorInfo;
class Shipyard;
class HyperspaceMenu;
class TrailManager;
class TrailManagerRep;
class ParticleManager;
class ParticleManagerRep;
class DeathMenu;
class LootWindow;
class Boundary;
class System;
class SplitRenderer;

static const float sSpawnPointSize = 1000.0f;
static const int sSectorSpawnPoints = 9;

using IntVector = std::vector<int>;
using ShipVector = std::vector<Ship*>;

class Sector
{
public:
    Sector(System* pSystem, const glm::vec2& coordinates);
    virtual ~Sector();
    void Update(float fDelta);

    bool Initialize();

    AmmoManager* GetAmmoManager() const;
    LaserManager* GetLaserManager() const;
    ParticleManager* GetParticleManager() const;
    MuzzleflashManager* GetMuzzleflashManager() const;
    const ShipList& GetShipList() const;
    ShipTweaks* GetShipTweaks() const;
    const glm::vec2& GetCoordinates() const;
    Entity* GetPlayerShip() const;

    void AddShip(Ship* pShip);
    void RemoveShip(Ship* pShip);

    bool IsPlayerVictorious() const;

private:
    void UpdateCameras();
    void CreatePlayerFleet();
    void CreateOtherFleet();
    void CreateOtherFleetViewport();

    void DeleteRemovedShips();
    bool GetFleetSpawnPosition(Faction* pFaction, float& x, float& y);
    void GetFleetSpawnPositionAtPoint(int idx, float& x, float& y);
    void DebugDrawFleetSpawnPositions();
    void SelectPlaylist();

    EntitySharedPtr CreateShip( const std::string& templateName, const glm::vec3& position, FleetSharedPtr& pFleet );

    System* m_pSystem;
    glm::vec2 m_Coordinates;

    Dust* m_pDust;
    Boundary* m_pBoundary;
    ShipList m_ShipList;
    ShipList m_ShipsToRemove;
    ParticleManager* m_pParticleManager;
    ParticleManagerRep* m_pParticleManagerRep;
    MuzzleflashManager* m_pMuzzleflashManager;
    MuzzleflashManagerRep* m_pMuzzleflashManagerRep;
    AmmoManager* m_pAmmoManager;
    LaserManager* m_pLaserManager;
    SpriteManager* m_pSpriteManager;
    TrailManager* m_pTrailManager;
    TrailManagerRep* m_pTrailManagerRep;
    SplitRenderer* m_pSplitRenderer;

    IntVector m_AvailableSpawnPoints;

    ShipSpawnDataVector m_DebugShipSpawnData;

    Shipyard* m_pShipyard;
    HyperspaceMenu* m_pHyperspaceMenu;
    DeathMenu* m_pDeathMenu;

    bool m_IsPlayerVictorious;

    TowerBonus m_TowerBonus[static_cast<unsigned int>(FactionId::Count)];
    float m_TowerBonusMagnitude[static_cast<unsigned int>(FactionId::Count)];

    LootWindow* m_pLootWindow;

    unsigned int m_AdditionalWaves;
    unsigned int m_AdditionalWavesSpawned;

    ShipTweaksUniquePtr m_pShipTweaks;
    std::vector<EntitySharedPtr> m_Entities;
    EntitySharedPtr m_pPlayerShip;

    UI2::WindowSharedPtr m_pSidebarWindow;

    FleetSharedPtr m_pPlayerFleet;
    FleetSharedPtr m_pOtherFleet;
    Genesis::ViewportSharedPtr m_pOtherFleetViewport;
    Genesis::SceneCameraSharedPtr m_pPlayerFleetCamera;
    Genesis::SceneCameraSharedPtr m_pOtherFleetCamera;
};

inline AmmoManager* Sector::GetAmmoManager() const
{
    return m_pAmmoManager;
}

inline LaserManager* Sector::GetLaserManager() const
{
    return m_pLaserManager;
}

inline const ShipList& Sector::GetShipList() const
{
    return m_ShipList;
}

inline ParticleManager* Sector::GetParticleManager() const
{
    return m_pParticleManager;
}

inline MuzzleflashManager* Sector::GetMuzzleflashManager() const
{
    return m_pMuzzleflashManager;
}

inline bool Sector::IsPlayerVictorious() const
{
    return m_IsPlayerVictorious;
}

inline ShipTweaks* Sector::GetShipTweaks() const
{
    return m_pShipTweaks.get();
}

inline const glm::vec2& Sector::GetCoordinates() const
{
    return m_Coordinates;
}

inline Entity* Sector::GetPlayerShip() const 
{
    return m_pPlayerShip.get();
}

} // namespace Hyperscape