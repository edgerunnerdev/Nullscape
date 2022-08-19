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
#include "fleet/fleet.fwd.h"
#include "ship/moduleinfo.h"
#include "ship/ship.fwd.h"

#include <list>
#include <memory>
#include <scene/layer.h>
#include <vector>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Entity);

class AmmoManager;
class MuzzleflashManager;
class MuzzleflashManagerRep;
class LaserManager;
class SpriteManager;
class SectorCamera;
class Hotbar;
class Dust;
class ShipInfo;
class Faction;
class Fleet;
class SectorInfo;
class Shipyard;
class HyperspaceMenu;
class TrailManager;
class TrailManagerRep;
class Radar;
class ParticleManager;
class ParticleManagerRep;
class DeathMenu;
class LootWindow;
class FleetCommand;
class Boundary;
class System;

using HotbarUniquePtr = std::unique_ptr<Hotbar>;
using FleetCommandUniquePtr = std::unique_ptr<FleetCommand>;
using FleetCommandVector = std::vector<FleetCommandUniquePtr>;

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
    SpriteManager* GetSpriteManager() const;
    ParticleManager* GetParticleManager() const;
    MuzzleflashManager* GetMuzzleflashManager() const;
    const ShipList& GetShipList() const;
    TrailManager* GetTrailManager() const;
    ShipTweaks* GetShipTweaks() const;
    SectorCamera* GetCamera() const;
    const glm::vec2& GetCoordinates() const;
    Entity* GetPlayerShip() const;

    void AddShip(Ship* pShip);
    void RemoveShip(Ship* pShip);

    FleetWeakPtr GetRegionalFleet() const;

    bool Reinforce(FleetSharedPtr pFleet, ShipVector* pSpawnedShips = nullptr);
    bool IsPlayerVictorious() const;
    void AddFleetCommand(FleetCommandUniquePtr pFleetCommand);

private:
    void DeleteRemovedShips();
    bool GetFleetSpawnPosition(Faction* pFaction, float& x, float& y);
    void GetFleetSpawnPositionAtPoint(int idx, float& x, float& y);
    void DebugDrawFleetSpawnPositions();
    void SelectPlaylist();

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
    SectorCamera* m_pCamera;
    Radar* m_pRadar;

    HotbarUniquePtr m_pHotbar;

    FleetSharedPtr m_pRegionalFleet;
    IntVector m_AvailableSpawnPoints;

    ShipSpawnDataVector m_DebugShipSpawnData;

    Shipyard* m_pShipyard;
    HyperspaceMenu* m_pHyperspaceMenu;
    DeathMenu* m_pDeathMenu;

    bool m_IsPlayerVictorious;

    TowerBonus m_TowerBonus[static_cast<unsigned int>(FactionId::Count)];
    float m_TowerBonusMagnitude[static_cast<unsigned int>(FactionId::Count)];

    LootWindow* m_pLootWindow;
    FleetCommandVector m_FleetCommands;

    unsigned int m_AdditionalWaves;
    unsigned int m_AdditionalWavesSpawned;
    FleetList m_TemporaryFleets;

    ShipTweaksUniquePtr m_pShipTweaks;
    std::vector<EntitySharedPtr> m_Entities;
    EntitySharedPtr m_pPlayerShip;
};

inline AmmoManager* Sector::GetAmmoManager() const
{
    return m_pAmmoManager;
}

inline LaserManager* Sector::GetLaserManager() const
{
    return m_pLaserManager;
}

inline SpriteManager* Sector::GetSpriteManager() const
{
    return m_pSpriteManager;
}

inline const ShipList& Sector::GetShipList() const
{
    return m_ShipList;
}

inline TrailManager* Sector::GetTrailManager() const
{
    return m_pTrailManager;
}

inline ParticleManager* Sector::GetParticleManager() const
{
    return m_pParticleManager;
}

inline MuzzleflashManager* Sector::GetMuzzleflashManager() const
{
    return m_pMuzzleflashManager;
}

inline FleetWeakPtr Sector::GetRegionalFleet() const
{
    return m_pRegionalFleet;
}

inline bool Sector::IsPlayerVictorious() const
{
    return m_IsPlayerVictorious;
}

inline ShipTweaks* Sector::GetShipTweaks() const
{
    return m_pShipTweaks.get();
}

inline SectorCamera* Sector::GetCamera() const
{
    return m_pCamera;
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