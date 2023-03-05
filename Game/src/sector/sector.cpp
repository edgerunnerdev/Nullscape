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
#include "entity/component.hpp"
#include "entity/componentfactory.hpp"
#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"
#include "entity/entityfactory.hpp"
#include "faction/faction.h"
#include "fleet/fleet.hpp"
#include "game.hpp"
#include "laser/lasermanager.h"
#include "menus/contextualtips.h"
#include "menus/deathmenu.h"
#include "menus/hyperspacemenu.h"
#include "menus/intelwindow.h"
#include "menus/lootwindow.h"
#include "menus/shiptweaks.h"
#include "muzzleflash/muzzleflashmanager.h"
#include "muzzleflash/muzzleflashmanagerrep.h"
#include "particles/particlemanager.h"
#include "particles/particlemanagerrep.h"
#include "player.h"
#include "sector/battlecontroller.hpp"
#include "sector/boundary.h"
#include "sector/dust.h"
#include "sector/splitrenderer.hpp"
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
#include "ui2/ui2.hpp"
#include "windows/sidebarwindow.hpp"

#include <configuration.h>
#include <genesis.h>
#include <inputmanager.h>
#include <log.hpp>
#include <math/misc.h>
#include <render/rendertarget.h>
#include <rendersystem.h>
#include <scene/light.h>
#include <scene/scene.h>
#include <scene/sceneobject.h>
#include <sound/soundinstance.h>
#include <sound/soundmanager.h>

// clang-format off
#include <externalheadersbegin.hpp>
#include <tinyxml2.h>
#include <externalheadersend.hpp>
// clang-format on

#include <algorithm>
#include <sstream>

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Sector
///////////////////////////////////////////////////////////////////////////////

Sector::Sector( System* pSystem, const glm::vec2& coordinates )
    : m_pSystem( pSystem )
    , m_Coordinates( coordinates )
    , m_pDust( nullptr )
    , m_pBoundary( nullptr )
    , m_pAmmoManager( nullptr )
    , m_pLaserManager( nullptr )
    , m_pSpriteManager( nullptr )
    , m_pTrailManager( nullptr )
    , m_pTrailManagerRep( nullptr )
    , m_pSplitRenderer( nullptr )
    , m_pShipyard( nullptr )
    , m_IsPlayerVictorious( false )
    , m_pLootWindow( nullptr )
    , m_AdditionalWaves( 0u )
    , m_AdditionalWavesSpawned( 0u )
{
    m_pHyperspaceMenu = new HyperspaceMenu();
    m_pDeathMenu = new DeathMenu();
    m_pLootWindow = new LootWindow();

    for ( int i = 0; i < (int)FactionId::Count; ++i )
    {
        m_TowerBonus[ i ] = TowerBonus::None;
        m_TowerBonusMagnitude[ i ] = 0.0f;
    }

    g_pGame->SetCursorType( CursorType::Crosshair );

    m_pShipTweaks = std::make_unique<ShipTweaks>();

    m_pSidebarWindow = UI2::OpenWindow<SidebarWindow>();
}

Sector::~Sector()
{
    DamageTrackerDebugWindow::Unregister();

    delete m_pHyperspaceMenu;
    delete m_pDeathMenu;
    delete m_pLootWindow;
    delete m_pTrailManager;

    if ( m_pParticleManager != nullptr )
    {
        if ( m_pParticleManagerRep != nullptr )
        {
            m_pParticleManagerRep->SetParticleManager( nullptr );
        }
        delete m_pParticleManager;
    }

    if ( m_pMuzzleflashManager != nullptr )
    {
        if ( m_pMuzzleflashManagerRep != nullptr )
        {
            m_pMuzzleflashManagerRep->SetManager( nullptr );
        }
        delete m_pMuzzleflashManager;
    }

    g_pGame->SetCursorType( CursorType::Pointer );
}

bool Sector::Initialize()
{
    // Setup the spawn points
    // This is effectively a 3x3 grid
    // We will ignore index 4 (central spawn point) as that is used for shipyards and other events
    for ( int i = 0; i < sSectorSpawnPoints; ++i )
    {
        if ( i != 4 )
        {
            m_AvailableSpawnPoints.push_back( i );
        }
    }

    m_pPlayerFleetCamera = Genesis::FrameWork::GetScene()->GetCamera();

    m_pTrailManager = new TrailManager();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pTrailManager );
    m_pTrailManagerRep = new TrailManagerRep( m_pTrailManager );
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pTrailManagerRep );

    m_pParticleManager = new ParticleManager();
    m_pParticleManagerRep = new ParticleManagerRep( m_pParticleManager );
    m_pSystem->GetLayer( LayerId::Effects )->AddSceneObject( m_pParticleManagerRep );

    m_pMuzzleflashManager = new MuzzleflashManager();
    m_pMuzzleflashManagerRep = new MuzzleflashManagerRep( m_pMuzzleflashManager );
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pMuzzleflashManagerRep );

    m_pDust = new Dust();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pDust );

    m_pBoundary = new Boundary();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pBoundary );

    m_pSystem->GetLayer( LayerId::Debug )->AddSceneObject( Genesis::FrameWork::GetDebugRender(), false );

    m_pAmmoManager = new AmmoManager();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pAmmoManager );

    m_pLaserManager = new LaserManager();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pLaserManager );

    m_pSpriteManager = new SpriteManager();
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pSpriteManager );

    DamageTrackerDebugWindow::Register();

    Genesis::LightArray& lights = Genesis::FrameWork::GetScene()->GetLights();
    lights[ 0 ].SetPosition( { 100.0f, 100.0f, 100.0f } );
    lights[ 1 ].SetPosition( { 100.0f, 0.0f, 0.0f } );
    lights[ 2 ].SetPosition( { -100.0f, 100.0f, -100.0f } );

    CreatePlayerFleet();
    CreateOtherFleet();
    CreateOtherFleetViewport();
    m_pSplitRenderer = new SplitRenderer( m_pOtherFleetViewport );
    m_pSystem->GetLayer( LayerId::Split )->AddSceneObject( m_pSplitRenderer );

    m_pBattleController = std::make_shared<BattleController>( m_pPlayerFleet, m_pOtherFleet );
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( m_pBattleController.get(), false );

    return true;
}

void Sector::CreatePlayerFleet()
{
    m_pPlayerFleet = std::make_shared<Fleet>();
    m_pPlayerShip = CreateShip( "dagger", glm::vec3(0.0f, 0.0f, 0.0f), m_pPlayerFleet);
    m_pPlayerShip->AddComponent( ComponentFactory::Get()->Create( ComponentType::PlayerControllerComponent ) );
    CreateShip( "dagger", glm::vec3(20.0f, 20.0f, -10.0f), m_pPlayerFleet);
    CreateShip( "dagger", glm::vec3(50.0f, -15.0f, -5.0f), m_pPlayerFleet);
}

void Sector::CreateOtherFleet()
{
    m_pOtherFleet = std::make_shared<Fleet>();
    CreateShip( "dagger", glm::vec3(0.0f, 0.0f, 1000.0f), m_pOtherFleet);
    CreateShip( "dagger", glm::vec3(20.0f, 20.0f, 1000.0f - 10.0f), m_pOtherFleet);
}

void Sector::CreateOtherFleetViewport()
{
    using namespace Genesis;
    m_pOtherFleetCamera = std::make_shared<SceneCamera>();
    FrameWork::GetScene()->AddCamera( m_pOtherFleetCamera );
    m_pOtherFleetViewport = std::make_shared<Viewport>( "Other fleet", static_cast<int>( Configuration::GetScreenWidth() ), static_cast<int>( Configuration::GetScreenHeight() ), FrameWork::GetScene(), m_pOtherFleetCamera );
    FrameWork::GetRenderSystem()->AddViewport( m_pOtherFleetViewport );
}

EntitySharedPtr Sector::CreateShip( const std::string& templateName, const glm::vec3& position, FleetSharedPtr& pFleet )
{
    SDL_assert( pFleet );
    EntitySharedPtr pShipEntity = EntityFactory::Get()->Create( templateName );
    if ( pShipEntity )
    {
        m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( pShipEntity.get(), false );
        m_Entities.push_back( pShipEntity );
        pFleet->AddShip( pShipEntity );
        pShipEntity->GetComponent<TransformComponent>()->SetTransform( glm::translate( position ) );
        return pShipEntity;
    }
    else
    {
        Genesis::Log::Error() << "Failed to create ship based on template '" << templateName << "'.";
        return nullptr;
    }
}

void Sector::SelectPlaylist()
{
    using namespace Genesis;

    bool flagshipPresent = false;
    const ShipList& shipList = GetShipList();
    for ( auto& pShip : shipList )
    {
        if ( Faction::sIsEnemyOf( pShip->GetFaction(), g_pGame->GetPlayerFaction() ) && pShip->IsFlagship() )
        {
            flagshipPresent = true;
            break;
        }
    }

    std::string playlist = flagshipPresent ? "data/playlists/bossfight.m3u" : "data/playlists/combat.m3u";
    ResourceSound* pPlaylistResource = (ResourceSound*)FrameWork::GetResourceManager()->GetResource( playlist );
    FrameWork::GetSoundManager()->SetPlaylist( pPlaylistResource, "", true );
}

void Sector::Update( float delta )
{
    UpdateCameras();

#ifndef _FINAL
    m_pShipTweaks->Update( delta );
#endif

    m_pLootWindow->Update( delta );

    // Draw axis.
    static bool sDrawAxis = false;
    if ( sDrawAxis )
    {

        Genesis::FrameWork::GetDebugRender()->DrawLine( glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 200.0f, 0.0f, 0.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
        Genesis::FrameWork::GetDebugRender()->DrawLine( glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, 200.0f, 0.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
        Genesis::FrameWork::GetDebugRender()->DrawLine( glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, 0.0f, 200.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
    }

    DeleteRemovedShips();

    if ( m_pShipTweaks->GetDrawFleetSpawnPositions() )
    {
        DebugDrawFleetSpawnPositions();
    }

    Ship* pPlayerShip = g_pGame->GetPlayer()->GetShip();
    if ( pPlayerShip != nullptr && pPlayerShip->GetDockingState() == DockingState::Undocked && !pPlayerShip->GetHyperspaceCore()->IsCharging() && !pPlayerShip->GetHyperspaceCore()->IsJumping() )
    {
        Genesis::InputManager* pInputManager = Genesis::FrameWork::GetInputManager();
        if ( pInputManager->IsButtonPressed( SDL_SCANCODE_ESCAPE ) )
        {
            m_pHyperspaceMenu->Show( true );
        }
    }

    m_pHyperspaceMenu->Update( delta );
    m_pDeathMenu->Update( delta );

    DamageTrackerDebugWindow::Update();
}

void Sector::UpdateCameras()
{
    static float sPlayerCameraPos[ 3 ] = { -40.0f, 25.0f, 35.0f };
    static float sPlayerCameraTarget[ 3 ] = { 20.0f, 10.0f, 25.0f };
    static float sOtherCameraPos[ 3 ] = { 55.0f, 45.0f, 1050.0f };
    static float sOtherCameraTarget[ 3 ] = { -40.0f, 10.0f, 1010.0f };
    static bool sDebugCamera = false;
    if ( sDebugCamera && ImGui::Begin( "Camera" ) )
    {
        ImGui::InputFloat3( "Player camera position", sPlayerCameraPos );
        ImGui::InputFloat3( "Player camera target", sPlayerCameraTarget );

        ImGui::SliderFloat3("Other camera position", sOtherCameraPos, -100.0f, 1200.0f );
        ImGui::SliderFloat3("Other camera target", sOtherCameraTarget, -100.0f, 1200.0f );
        ImGui::End();
    }

    m_pPlayerFleetCamera->SetPosition( glm::vec3( sPlayerCameraPos[ 0 ], sPlayerCameraPos[ 1 ], sPlayerCameraPos[ 2 ] ) );
    m_pPlayerFleetCamera->SetTargetPosition( glm::vec3( sPlayerCameraTarget[ 0 ], sPlayerCameraTarget[ 1 ], sPlayerCameraTarget[ 2 ] ) );
    m_pOtherFleetCamera->SetPosition( glm::vec3( sOtherCameraPos[ 0 ], sOtherCameraPos[ 1 ], sOtherCameraPos[ 2 ] ) );
    m_pOtherFleetCamera->SetTargetPosition( glm::vec3( sOtherCameraTarget[ 0 ], sOtherCameraTarget[ 1 ], sOtherCameraTarget[ 2 ] ) );
}

void Sector::DeleteRemovedShips()
{
    for ( Ship* pShip : m_ShipsToRemove )
    {
        m_pSystem->GetLayer( LayerId::Ships )->RemoveSceneObject( pShip );
        m_ShipList.remove( pShip );
    }

    m_ShipsToRemove.clear();
}

void Sector::AddShip( Ship* pShip )
{
    m_ShipList.push_back( pShip );
    m_pSystem->GetLayer( LayerId::Ships )->AddSceneObject( pShip, true );

    // If non-Imperial ships arrive after victory, then the victory has to be rescinded
    if ( m_IsPlayerVictorious && pShip->GetFaction() != g_pGame->GetFaction( FactionId::Empire ) )
    {
        m_IsPlayerVictorious = false;
    }

    // Flavour text for flagships
    if ( pShip->IsFlagship() )
    {
        FactionId factionId = pShip->GetFaction()->GetFactionId();
        if ( factionId == FactionId::Ascent )
        {
            g_pGame->AddFleetCommandIntel( "The Ascent flagship is here, the 'Angel of Io'. It is heavily shielded and has a devastating battery of lances, as well as two forward-mounted ion cannons. "
                                           "Iriani technology, we reckon." );
        }
        else if ( factionId == FactionId::Pirate )
        {
            g_pGame->AddFleetCommandIntel( "The Pirate flagship is here, Captain. The 'Northern Star' is a commandeered vessel from an unaligned sector and uses experimental lance weapons." );
        }
        else if ( factionId == FactionId::Marauders )
        {
            g_pGame->AddFleetCommandIntel( "Presence of the Marauder flagship is confirmed. The 'Ragnarokkr' is extremely heavily armoured and relies on artilleries and torpedo launchers." );
            g_pGame->AddFleetCommandIntel( "From previous battles we know it uses reactive armour, so energy weapons would be more effective against it." );
        }
        else if ( factionId == FactionId::Iriani )
        {
            // Silver Handmaiden
            // g_pGame->AddFleetCommandIntel( "" );
        }
    }
}

void Sector::RemoveShip( Ship* pShip )
{
    // Check if we already have this ship in our list of ships to remove
    const ShipList::iterator& itEnd = m_ShipsToRemove.end();
    for ( ShipList::iterator it = m_ShipsToRemove.begin(); it != itEnd; ++it )
    {
        if ( *it == pShip )
            return;
    }

    pShip->Terminate();

    m_ShipsToRemove.push_back( pShip );
}

bool Sector::GetFleetSpawnPosition( Faction* pFleetFaction, float& x, float& y )
{
    if ( m_AvailableSpawnPoints.empty() )
    {
        return false;
    }
    else
    {
        int idx = rand() % m_AvailableSpawnPoints.size();
        int selectedSpawnPoint = m_AvailableSpawnPoints[ idx ];
        m_AvailableSpawnPoints.erase( m_AvailableSpawnPoints.begin() + idx );

        GetFleetSpawnPositionAtPoint( selectedSpawnPoint, x, y );

        return true;
    }
}

// Returns the center point of a tile in a square containing sSectorSpawnPoints tiles.
void Sector::GetFleetSpawnPositionAtPoint( int idx, float& x, float& y )
{
    static const int sSide = static_cast<int>( sqrt( sSectorSpawnPoints ) );
    x = static_cast<float>( idx % sSide ) * sSpawnPointSize + sSpawnPointSize * 0.5f - sSide / 2.0f * sSpawnPointSize;
    y = floor( static_cast<float>( idx ) / sSide ) * sSpawnPointSize + sSpawnPointSize * 0.5f - sSide / 2.0f * sSpawnPointSize;
}

void Sector::DebugDrawFleetSpawnPositions()
{
    float x, y;
    float spawnPointRadius = sSpawnPointSize * 0.5f;
    spawnPointRadius *= 0.99f; // Just so the circles don't touch each other while drawing
    for ( int i = 0; i < sSectorSpawnPoints; ++i )
    {
        GetFleetSpawnPositionAtPoint( i, x, y );
        Genesis::FrameWork::GetDebugRender()->DrawCircle( glm::vec2( x, y ), 100.0f, glm::vec3( 0.3f, 0.3f, 1.0f ) );
        Genesis::FrameWork::GetDebugRender()->DrawCircle( glm::vec2( x, y ), spawnPointRadius, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    }

    for ( ShipList::const_iterator it = m_ShipList.cbegin(), itEnd = m_ShipList.cend(); it != itEnd; ++it )
    {
        const ShipSpawnData& spawnData = ( *it )->GetShipSpawnData();
        glm::vec2 origin( spawnData.m_PositionX, spawnData.m_PositionY );
        Genesis::FrameWork::GetDebugRender()->DrawCircle( origin, 25.0f, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    }
}

} // namespace Hyperscape