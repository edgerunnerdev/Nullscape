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

#include "sector.h"

#include "achievements.h"
#include "ammo/ammomanager.h"
#include "entity/entity.hpp"
#include "entity/component.hpp"
#include "entity/componentfactory.hpp"
#include "entity/entityfactory.hpp"
#include "faction/faction.h"
#include "fleet/fleet.h"
#include "fleet/fleetcommand.h"
#include "fleet/fleetspawner.h"
#include "hexterminate.h"
#include "laser/lasermanager.h"
#include "menus/contextualtips.h"
#include "menus/deathmenu.h"
#include "menus/hotbar.h"
#include "menus/hyperspacemenu.h"
#include "menus/intelwindow.h"
#include "menus/lootwindow.h"
#include "menus/radar.h"
#include "menus/shiptweaks.h"
#include "muzzleflash/muzzleflashmanager.h"
#include "muzzleflash/muzzleflashmanagerrep.h"
#include "particles/particlemanager.h"
#include "particles/particlemanagerrep.h"
#include "player.h"
#include "sector/boundary.h"
#include "sector/dust.h"
#include "sector/sectorcamera.h"
#include "ship/collisionmasks.h"
#include "ship/damagetracker.h"
#include "ship/hyperspacecore.h"
#include "ship/ship.h"
#include "ship/shipinfo.h"
#include "shipyard/shipyard.h"
#include "sprite/spritemanager.h"
#include "system/system.hpp"
#include "trail/trailmanager.h"
#include "trail/trailmanagerrep.h"

#include <algorithm>
#include <beginexternalheaders.h>
#include <endexternalheaders.h>
#include <genesis.h>
#include <inputmanager.h>
#include <log.hpp>
#include <math/misc.h>
#include <render/debugrender.h>
#include <scene/layer.h>
#include <scene/light.h>
#include <scene/scene.h>
#include <scene/sceneobject.h>
#include <sound/soundinstance.h>
#include <sound/soundmanager.h>
#include <sstream>
#include <tinyxml2.h>

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Sector
///////////////////////////////////////////////////////////////////////////////

Sector::Sector(System* pSystem, const glm::vec2& coordinates)
    : m_pSystem(pSystem)
    , m_Coordinates(coordinates)
    , m_pDust(nullptr)
    , m_pBoundary(nullptr)
    , m_pAmmoManager(nullptr)
    , m_pLaserManager(nullptr)
    , m_pSpriteManager(nullptr)
    , m_pTrailManager(nullptr)
    , m_pTrailManagerRep(nullptr)
    , m_pRadar(nullptr)
    , m_pRegionalFleet(nullptr)
    , m_pShipyard(nullptr)
    , m_IsPlayerVictorious(false)
    , m_pLootWindow(nullptr)
    , m_AdditionalWaves(0u)
    , m_AdditionalWavesSpawned(0u)
{
    m_pCamera = new SectorCamera();

    m_pHyperspaceMenu = new HyperspaceMenu();
    m_pDeathMenu = new DeathMenu();
    m_pLootWindow = new LootWindow();

    m_pHotbar = std::make_unique<Hotbar>();

    for (int i = 0; i < (int)FactionId::Count; ++i)
    {
        m_TowerBonus[i] = TowerBonus::None;
        m_TowerBonusMagnitude[i] = 0.0f;
    }

    g_pGame->SetCursorType(CursorType::Crosshair);

    m_pShipTweaks = std::make_unique<ShipTweaks>();
}

Sector::~Sector()
{
    DamageTrackerDebugWindow::Unregister();

    delete m_pCamera;
    delete m_pHyperspaceMenu;
    delete m_pDeathMenu;
    delete m_pLootWindow;
    delete m_pTrailManager;

    if (m_pParticleManager != nullptr)
    {
        if (m_pParticleManagerRep != nullptr)
        {
            m_pParticleManagerRep->SetParticleManager(nullptr);
        }
        delete m_pParticleManager;
    }

    if (m_pMuzzleflashManager != nullptr)
    {
        if (m_pMuzzleflashManagerRep != nullptr)
        {
            m_pMuzzleflashManagerRep->SetManager(nullptr);
        }
        delete m_pMuzzleflashManager;
    }

    if (m_pRadar != nullptr)
    {
        Genesis::FrameWork::GetGuiManager()->RemoveElement(m_pRadar);
    }

    g_pGame->SetCursorType(CursorType::Pointer);
}

bool Sector::Initialize()
{
    // Setup the spawn points
    // This is effectively a 3x3 grid
    // We will ignore index 4 (central spawn point) as that is used for shipyards and other events
    for (int i = 0; i < sSectorSpawnPoints; ++i)
    {
        if (i != 4)
        {
            m_AvailableSpawnPoints.push_back(i);
        }
    }

    m_pTrailManager = new TrailManager();
    m_pTrailManagerRep = new TrailManagerRep(m_pTrailManager);
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(m_pTrailManagerRep);

    m_pParticleManager = new ParticleManager();
    m_pParticleManagerRep = new ParticleManagerRep(m_pParticleManager);
    m_pSystem->GetLayer(LayerId::Effects)->AddSceneObject(m_pParticleManagerRep);

    m_pMuzzleflashManager = new MuzzleflashManager();
    m_pMuzzleflashManagerRep = new MuzzleflashManagerRep(m_pMuzzleflashManager);
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(m_pMuzzleflashManagerRep);

    m_pDust = new Dust();
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(m_pDust);

    m_pBoundary = new Boundary();
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(m_pBoundary);

    m_pSystem->GetLayer(LayerId::Physics)->AddSceneObject(Genesis::FrameWork::GetDebugRender(), false);

    m_pAmmoManager = new AmmoManager();
    m_pSystem->GetLayer(LayerId::Ammo)->AddSceneObject(m_pAmmoManager);

    m_pLaserManager = new LaserManager();
    m_pSystem->GetLayer(LayerId::Ammo)->AddSceneObject(m_pLaserManager);

    m_pSpriteManager = new SpriteManager();
    m_pSystem->GetLayer(LayerId::Ammo)->AddSceneObject(m_pSpriteManager);

    m_pRadar = new Radar();
    Genesis::FrameWork::GetGuiManager()->AddElement(m_pRadar);

    DamageTrackerDebugWindow::Register();

    Genesis::LightArray& lights = Genesis::FrameWork::GetScene()->GetLights();
    lights[0].SetPosition({100.0f, 100.0f, 100.0f});
    lights[1].SetPosition({100.0f, 0.0f, 0.0f});
    lights[2].SetPosition({-100.0f, 100.0f, -100.0f});

    EntitySharedPtr pShipEntity = EntityFactory::Get()->Create("dagger");
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(pShipEntity.get(), false);
    m_Entities.push_back(pShipEntity);
    m_pPlayerShip = pShipEntity;

    return true;
}

void Sector::SelectPlaylist()
{
    using namespace Genesis;

    bool flagshipPresent = false;
    const ShipList& shipList = GetShipList();
    for (auto& pShip : shipList)
    {
        if (Faction::sIsEnemyOf(pShip->GetFaction(), g_pGame->GetPlayerFaction()) && pShip->IsFlagship())
        {
            flagshipPresent = true;
            break;
        }
    }

    std::string playlist = flagshipPresent ? "data/playlists/bossfight.m3u" : "data/playlists/combat.m3u";
    ResourceSound* pPlaylistResource = (ResourceSound*)FrameWork::GetResourceManager()->GetResource(playlist);
    FrameWork::GetSoundManager()->SetPlaylist(pPlaylistResource, "", true);
}

void Sector::Update(float delta)
{
#ifndef _FINAL
    m_pShipTweaks->Update(delta);
#endif

    m_pTrailManager->Update(delta);
    m_pAmmoManager->Update(delta);
    m_pLaserManager->Update(delta);
    m_pSpriteManager->Update(delta);
    m_pParticleManager->Update(delta);
    m_pMuzzleflashManager->Update(delta);
    m_pCamera->Update(delta);
    m_pLootWindow->Update(delta);

    DeleteRemovedShips();

    if (m_pShipTweaks->GetDrawFleetSpawnPositions())
    {
        DebugDrawFleetSpawnPositions();
    }

    Ship* pPlayerShip = g_pGame->GetPlayer()->GetShip();
    if (pPlayerShip != nullptr && pPlayerShip->GetDockingState() == DockingState::Undocked && !pPlayerShip->GetHyperspaceCore()->IsCharging() && !pPlayerShip->GetHyperspaceCore()->IsJumping())
    {
        Genesis::InputManager* pInputManager = Genesis::FrameWork::GetInputManager();
        if (pInputManager->IsButtonPressed(SDL_SCANCODE_ESCAPE))
        {
            m_pHyperspaceMenu->Show(true);
        }
    }

    m_pHyperspaceMenu->Update(delta);
    m_pDeathMenu->Update(delta);

    for (auto& pFleetCommand : m_FleetCommands)
    {
        pFleetCommand->Update();
    }

    DamageTrackerDebugWindow::Update();
}

void Sector::DeleteRemovedShips()
{
    for (Ship* pShip : m_ShipsToRemove)
    {
        m_pSystem->GetLayer(LayerId::Ships)->RemoveSceneObject(pShip);
        m_ShipList.remove(pShip);
    }

    m_ShipsToRemove.clear();
}

bool Sector::Reinforce(FleetSharedPtr pFleet, ShipVector* pSpawnedShips /* = nullptr */)
{
    if (pFleet == nullptr)
    {
        return false;
    }

    float spawnPointX, spawnPointY;
    if (GetFleetSpawnPosition(pFleet->GetFaction(), spawnPointX, spawnPointY))
    {
        FleetSpawner::Spawn(pFleet, this, pSpawnedShips, spawnPointX, spawnPointY);

        if (Faction::sIsEnemyOf(pFleet->GetFaction(), g_pGame->GetPlayerFaction()))
        {
            g_pGame->AddFleetCommandIntel("Detected waveform collapse, an enemy fleet is entering the sector.");
        }
        else
        {
            g_pGame->AddFleetCommandIntel("Captain, reinforcements have arrived.");
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Sector::AddShip(Ship* pShip)
{
    m_ShipList.push_back(pShip);
    m_pSystem->GetLayer(LayerId::Ships)->AddSceneObject(pShip, true);

    // If non-Imperial ships arrive after victory, then the victory has to be rescinded
    if (m_IsPlayerVictorious && pShip->GetFaction() != g_pGame->GetFaction(FactionId::Empire))
    {
        m_IsPlayerVictorious = false;
    }

    // Flavour text for flagships
    if (pShip->IsFlagship())
    {
        FactionId factionId = pShip->GetFaction()->GetFactionId();
        if (factionId == FactionId::Ascent)
        {
            g_pGame->AddFleetCommandIntel("The Ascent flagship is here, the 'Angel of Io'. It is heavily shielded and has a devastating battery of lances, as well as two forward-mounted ion cannons. "
                                          "Iriani technology, we reckon.");
        }
        else if (factionId == FactionId::Pirate)
        {
            g_pGame->AddFleetCommandIntel("The Pirate flagship is here, Captain. The 'Northern Star' is a commandeered vessel from an unaligned sector and uses experimental lance weapons.");
        }
        else if (factionId == FactionId::Marauders)
        {
            g_pGame->AddFleetCommandIntel("Presence of the Marauder flagship is confirmed. The 'Ragnarokkr' is extremely heavily armoured and relies on artilleries and torpedo launchers.");
            g_pGame->AddFleetCommandIntel("From previous battles we know it uses reactive armour, so energy weapons would be more effective against it.");
        }
        else if (factionId == FactionId::Iriani)
        {
            // Silver Handmaiden
            // g_pGame->AddFleetCommandIntel( "" );
        }
    }
}

void Sector::RemoveShip(Ship* pShip)
{
    // Check if we already have this ship in our list of ships to remove
    const ShipList::iterator& itEnd = m_ShipsToRemove.end();
    for (ShipList::iterator it = m_ShipsToRemove.begin(); it != itEnd; ++it)
    {
        if (*it == pShip)
            return;
    }

    pShip->Terminate();

    m_ShipsToRemove.push_back(pShip);
}

bool Sector::GetFleetSpawnPosition(Faction* pFleetFaction, float& x, float& y)
{
    if (m_AvailableSpawnPoints.empty())
    {
        return false;
    }
    else
    {
        int idx = rand() % m_AvailableSpawnPoints.size();
        int selectedSpawnPoint = m_AvailableSpawnPoints[idx];
        m_AvailableSpawnPoints.erase(m_AvailableSpawnPoints.begin() + idx);

        GetFleetSpawnPositionAtPoint(selectedSpawnPoint, x, y);

        return true;
    }
}

// Returns the center point of a tile in a square containing sSectorSpawnPoints tiles.
void Sector::GetFleetSpawnPositionAtPoint(int idx, float& x, float& y)
{
    static const int sSide = static_cast<int>(sqrt(sSectorSpawnPoints));
    x = static_cast<float>(idx % sSide) * sSpawnPointSize + sSpawnPointSize * 0.5f - sSide / 2.0f * sSpawnPointSize;
    y = floor(static_cast<float>(idx) / sSide) * sSpawnPointSize + sSpawnPointSize * 0.5f - sSide / 2.0f * sSpawnPointSize;
}

void Sector::DebugDrawFleetSpawnPositions()
{
    float x, y;
    float spawnPointRadius = sSpawnPointSize * 0.5f;
    spawnPointRadius *= 0.99f; // Just so the circles don't touch each other while drawing
    for (int i = 0; i < sSectorSpawnPoints; ++i)
    {
        GetFleetSpawnPositionAtPoint(i, x, y);
        Genesis::FrameWork::GetDebugRender()->DrawCircle(glm::vec2(x, y), 100.0f, glm::vec3(0.3f, 0.3f, 1.0f));
        Genesis::FrameWork::GetDebugRender()->DrawCircle(glm::vec2(x, y), spawnPointRadius, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    for (ShipList::const_iterator it = m_ShipList.cbegin(), itEnd = m_ShipList.cend(); it != itEnd; ++it)
    {
        const ShipSpawnData& spawnData = (*it)->GetShipSpawnData();
        glm::vec2 origin(spawnData.m_PositionX, spawnData.m_PositionY);
        Genesis::FrameWork::GetDebugRender()->DrawCircle(origin, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }
}

void Sector::AddFleetCommand(FleetCommandUniquePtr pFleetCommand)
{
    m_FleetCommands.push_back(std::move(pFleetCommand));
}

} // namespace Hyperscape