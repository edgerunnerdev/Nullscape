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

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include "Shlwapi.h"
#pragma comment (lib, "Shlwapi.lib")
#endif

#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>

#include <imgui.h>

#include <genesis.h>
#include <taskmanager.h>
#include <gui/gui.h>
#include <gui/video.h>
#include <imgui/imgui_impl.h>
#include <physics/simulation.h>
#include <render/debugrender.h>
#include <resourcemanager.h>
#include <resources/resourcemodel.h>
#include <resources/resourcesound.h>
#include <resources/resourcevideo.h>
#include <sound/soundmanager.h>
#include <viewers/modelviewer/modelviewer.hpp>
#include <rendersystem.h>
#include <log.hpp>
#include <configuration.h>
#include <memory.h>
#include <stringaux.h>
#include <time.h>
#include <timer.h>
#include <videoplayer.h>
#include <xml.h>
#include <window.h>

#include "hexterminate.h"
#include "faction/faction.h"
#include "misc/gui.h"
#include "misc/random.h"
#include "ship/module.h"
#include "ship/shipinfo.h"
#include "ship/ship.h"
#include "ship/inventory.h"
#include "ship/shipoutline.h"
#include "fleet/fleet.h"
#include "sector/sectorinfo.h"
#include "sector/sector.h"
#include "system/system.hpp"
#include "system/systemviewer.hpp"
#include "ui/editor.h"
#include "ui/rootelement.h"
#include "viewers/entityviewer/entityviewer.hpp"
#include "viewers/explorationviewer/explorationviewer.hpp"
#include "menus/mainmenu.h"
#include "menus/audiodebug.h"
#include "menus/intelwindow.h"
#include "menus/loadingscreen.h"
#include "menus/pointofinterest.h"
#include "menus/popup.h"
#include "menus/tutorialwindow.h"
#include "misc/randomshuffle.h"
#include "achievements.h"
#include "perks.h"
#include "player.h"
#include "savegameheader.h"
#include "savegamestorage.h"
#include "shadertweaks.h"
#include "stringaux.h"
#include "xmlaux.h"

#include "particles/particlemanager.h"
#include "particles/particleemitter.h"

namespace Hyperscape {

Game* g_pGame = nullptr;

//-------------------------------------------------------------------
// Game
//-------------------------------------------------------------------

Game::Game():
m_pMainMenu( nullptr ),
m_pConsole( nullptr ),
m_pAudioDebug( nullptr ),
m_pPlayer( nullptr ),
m_pMusicTitle( nullptr ),
m_pTutorialWindow( nullptr ),
m_State( GameState::Menu ),
m_pIntelWindow( nullptr ),
m_pAchievementsManager ( nullptr ),
m_PlayedTime( 0.0f ),
m_IsPaused( false ),
m_IsInputBlocked( false ),
m_InputBlockedTimer( 0.0f ),
m_pFrameText( nullptr ),
m_ContextualTipsEnabled( true ),
m_QuitRequested( false ),
m_pVideoElement( nullptr ),
m_CursorType( CursorType::Pointer ),
m_LoadToState( GameState::Unknown ),
m_KillSave( false ),
m_ShowImguiTestWindow( false ),
m_AllResourcesLoaded( false ),
m_pModuleInfoManager( nullptr ),
m_pPhysicsSimulation( nullptr ),
m_pPopup( nullptr ),
m_pShipInfoManager( nullptr )
{
	using namespace Genesis;

	srand( (unsigned int)time( nullptr ) );

	TaskManager* taskManager = FrameWork::GetTaskManager();
	taskManager->AddTask( "GameLoop", this, (TaskFunc)&Game::Update, TaskPriority::GameLogic );

	for ( int i = 0; i < (int)FactionId::Count; ++i )
	{
		m_pFaction[ i ] = nullptr;
	}

#ifndef _FINAL
	InputManager* pInputManager = FrameWork::GetInputManager();
	m_ImGuiToggleToken = pInputManager->AddKeyboardCallback( std::bind( &Game::ToggleImGui, this ), SDL_SCANCODE_F1, ButtonState::Pressed );
#endif
}

Game::~Game()
{
	ShaderTweaksDebugWindow::Unregister();

	delete m_pTutorialWindow;
	delete m_pConsole;
	delete m_pAudioDebug;
	delete m_pMainMenu;
	delete m_pPopup;
	delete m_pShipInfoManager;
	delete m_pMusicTitle;
	delete m_pModuleInfoManager;
	delete m_pIntelWindow;
	delete m_pAchievementsManager;

	for ( unsigned int i = 0; i < (unsigned int)FactionId::Count; ++i )
	{
		delete m_pFaction[ i ];
	}

	delete m_pPhysicsSimulation;

#ifndef _FINAL
	Genesis::InputManager* pInputManager = Genesis::FrameWork::GetInputManager();
	if ( pInputManager != nullptr )
	{
		pInputManager->RemoveKeyboardCallback( m_ImGuiToggleToken );
	}
#endif

	if ( m_LoaderThread.joinable() )
	{
		m_LoaderThread.join();
	}

#if USE_STEAM
	SteamAPI_Shutdown();
#endif
}

void Game::Initialise()
{
	Random::Initialise();
	RandomShuffle::Initialise();

	Genesis::Core::Log::Info() << "Hyperscape build " << HYPERSCAPE_BUILD;

	m_pLoadingScreen = LoadingScreenUniquePtr( new LoadingScreen );
	m_pBlackboard = std::make_shared<Blackboard>();
	m_pModuleInfoManager = new ModuleInfoManager();
	m_pShipInfoManager = new ShipInfoManager();

	m_pShipInfoManager->Initialise();

	m_pPhysicsSimulation = new Genesis::Physics::Simulation();
	Genesis::FrameWork::GetTaskManager()->AddTask( 
		"Physics", 
		m_pPhysicsSimulation, 
		(Genesis::TaskFunc)&Genesis::Physics::Simulation::Update, 
		Genesis::TaskPriority::Physics 
	);

	m_pPopup = new Popup();

#ifdef _DEBUG
	m_pAudioDebug = new AudioDebug();
#endif

	m_pMusicTitle = new MusicTitle();

#ifndef _FINAL
    m_pFrameText = GuiExtended::CreateText( 8, 8, 1024, 128, "", nullptr );
	m_pFrameText->SetColour( 1.0f, 0.4f, 0.0f, 1.0f ); 
#endif

	SetCursorType( CursorType::Pointer );

	ShaderTweaksDebugWindow::Register();
    m_pEntityViewer = std::make_unique<EntityViewer>();

    using namespace Genesis;
	if ( FrameWork::GetCommandLineParameters()->HasParameter("--no-intro") == false )
	{
		ResourceVideo* pWingsOfSteelVideo = (ResourceVideo*)FrameWork::GetResourceManager()->GetResource( "data/videos/WingsOfSteel.ivf" );
		pWingsOfSteelVideo->SetSkippable( true );
		Genesis::FrameWork::GetVideoPlayer()->Play( pWingsOfSteelVideo );
	}

#if USE_STEAM
	if ( SteamAPI_RestartAppIfNecessary( STEAM_APP_ID ) )
	{
		Quit();
		return;
	}

	if ( SteamAPI_Init() )
	{
        Genesis::Core::Log::Info() << "Steam API initialized.";
	}
	else
	{
        Genesis::Core::Log::Warning() << "SteamAPI_Init() failed.";
	}
#endif // USE_STEAM

    Genesis::ImGuiImpl::RegisterMenu( "Tools", "ImGui test window", &m_ShowImguiTestWindow );

	m_pAchievementsManager = new AchievementsManager();
	m_pSaveGameStorage = std::make_unique< SaveGameStorage >();
	m_pShipOutline = std::make_unique<ShipOutline>();
    m_pUIRootElement = std::make_unique<UI::RootElement>();
    m_pUIEditor = std::make_unique<UI::Editor>();
    m_pExplorationViewer = std::make_unique<ExplorationViewer>();
	m_pModelViewer = std::make_unique<Genesis::ModelViewer>();
    m_pSystemViewer = std::make_unique<SystemViewer>();

	SetState( GameState::Intro );
}

SaveGameStorage* Game::GetSaveGameStorage() const
{
	return m_pSaveGameStorage.get();
}

void Game::ToggleImGui()
{
#ifndef _FINAL
	Genesis::ImGuiImpl::Enable( !Genesis::ImGuiImpl::IsEnabled() );
#endif
}

void Game::SetInputBlocked( bool state )
{
	m_IsInputBlocked = state;
	
	if ( m_IsInputBlocked )
		m_InputBlockedTimer = 0.2f;
}

bool Game::IsInputBlocked() const
{
	return m_InputBlockedTimer > 0.0f || Genesis::ImGuiImpl::IsEnabled();
}

Genesis::TaskStatus Game::Update( float delta ) 
{
#if USE_STEAM
	SteamAPI_RunCallbacks();
#endif

    if ( m_ShowImguiTestWindow )
    {
        ImGui::ShowDemoWindow( &m_ShowImguiTestWindow );
    }

	m_pUIRootElement->Update();
    m_pUIEditor->UpdateDebugUI();
	m_pModelViewer->UpdateDebugUI();
    m_pExplorationViewer->UpdateDebugUI();
    m_pSystemViewer->UpdateDebugUI();
	GetBlackboard()->UpdateDebugUI();
	GetSaveGameStorage()->UpdateDebugUI();
	m_pEntityViewer->UpdateDebugUI();
	ShaderTweaksDebugWindow::Update();

	if ( GetState() == GameState::LoadResources )
	{
		if ( m_AllResourcesLoaded )
		{
			m_pLoadingScreen->Show( false );
			SetState( GameState::Menu );
		}
		else
		{
			m_pLoadingScreen->Update();
		}
	}

	if ( m_LoadToState != GameState::Unknown )
	{
		m_pLoadingScreen->Show( false );
		SetState( m_LoadToState );
		m_LoadToState = GameState::Unknown;
	}

	if ( m_GameToLoad.empty() == false )
	{
		LoadGameAux();
	}

	if ( m_IsInputBlocked == false && m_InputBlockedTimer > 0.0f )
	{
		m_InputBlockedTimer = std::max( 0.0f, m_InputBlockedTimer - delta );
	}

	if ( m_pPopup )
	{
		m_pPopup->Update( delta );
	}

	if ( m_pTutorialWindow )
	{
		m_pTutorialWindow->Update( delta );
	}

	if ( m_pMusicTitle )
	{
		m_pMusicTitle->Update( delta );
	}

	if ( m_pSystem )
	{
		m_pSystem->Update(delta);
	}

	if ( m_pIntelWindow )
	{
		m_pIntelWindow->Update( delta );
	}

	if ( m_pAchievementsManager )
	{
		m_pAchievementsManager->Update();
	}

	if ( m_pAudioDebug )
	{
		m_pAudioDebug->Update( delta );
	}

	Genesis::InputManager* pInputManager = Genesis::FrameWork::GetInputManager();
	static bool sConsoleToggle = false;
	if ( pInputManager->IsButtonPressed( SDL_SCANCODE_GRAVE ) && !sConsoleToggle )
	{
		if ( m_pAudioDebug != nullptr )
		{
			m_pAudioDebug->Show( !m_pAudioDebug->IsVisible() );
		}

		sConsoleToggle = true;
	}
	else if ( !pInputManager->IsButtonPressed( SDL_SCANCODE_GRAVE ) )
	{
		sConsoleToggle = false;
	}

	if ( GetState() == GameState::GalaxyView ||
		 GetState() == GameState::HyperscapeView ||
		 GetState() == GameState::Combat || 
		 GetState() == GameState::Shipyard )
	{
		if (IsPaused() == false)
		{
			m_PlayedTime += delta;
		}
	}

	if ( GetState() == GameState::Intro && Genesis::FrameWork::GetVideoPlayer()->IsPlaying() == false )
	{
		SetState( GameState::LoadResources );
	}

	return Genesis::TaskStatus::Continue;
}

void Game::StartNewGame(const ShipCustomisationData& customisationData)
{
	SDL_assert( GetPlayer() == nullptr );

	m_pPlayer = std::make_shared<Player>(customisationData);
	m_pMainMenu->Show(false);

	SetPlayedTime( 0.0f );

	m_pSystem = nullptr;
	m_pSystem = std::make_shared<System>("17260877307600676");
    m_pSystemViewer->View(m_pSystem);
    m_pExplorationViewer->View(m_pSystem);

	m_pSystem->JumpTo(m_pPlayer, {0, 0});

    SetState(GameState::Combat);
}

void Game::EndGame()
{

}

void Game::EndGameAux()
{
	SaveGame();

	delete m_pTutorialWindow;

	if ( m_pIntelWindow != nullptr )
	{
		m_pIntelWindow->Clear();
	}

	m_pPlayer = nullptr;

	m_pMainMenu->Show( true );
    m_pMainMenu->SetOption( MainMenuOption::NewGame );

	SetState( GameState::Menu );
}

void Game::KillSaveGame()
{
	m_KillSave = true;
}

bool Game::SaveGame()
{
	return GetSaveGameStorage()->SaveGame( m_KillSave );
}

void Game::LoadGame( SaveGameHeaderWeakPtr pSaveGameHeaderWeakPtr )
{
	SaveGameHeaderSharedPtr pSaveGameHeader = pSaveGameHeaderWeakPtr.lock();
	if ( pSaveGameHeader == nullptr )
	{
		g_pGame->RaiseInteractiveWarning( "Couldn't load game, header is null." );
		return;
	}

	if ( pSaveGameHeader->IsValid() == false )
    {
		g_pGame->RaiseInteractiveWarning( "Couldn't load game, header is invalid." );
		return;
    }

	m_pLoadingScreen->Show( true );
	m_GameToLoad = pSaveGameHeader->GetFilename();
}

void Game::LoadGameAux()
{
	using namespace tinyxml2;

	if ( m_GameToLoad.empty() )
	{
		return;
	}

	std::filesystem::path filename = m_GameToLoad;
	m_GameToLoad.clear();

	tinyxml2::XMLDocument xmlDoc;
	if ( m_pSaveGameStorage->LoadGame( filename, xmlDoc ) == false )
	{
		return;
	}

	bool hasErrors = false;
	XMLElement* pRootElem = xmlDoc.FirstChildElement();

	if ( !hasErrors )
	{
		// Load the Player and therefore the Inventory / ship's hexgrid template
		for ( XMLElement* pElem = pRootElem->FirstChildElement(); pElem != nullptr; pElem = pElem->NextSiblingElement() ) 
		{
			const std::string value( pElem->Value() );
			if ( value == "Player" )
			{
				m_pPlayer = std::make_shared<Player>();
				hasErrors = ( m_pPlayer->Read( pElem ) == false );
				break;
			}
		}
	}

	if ( !hasErrors )
	{
		m_pMainMenu->Show( false );
        m_ContextualTipsEnabled = m_pBlackboard->Exists( "#contextual_tips" );
		SetState( GameState::GalaxyView );
	}
	else
	{
		m_pPlayer = nullptr;
		RaiseInteractiveWarning( "Invalid save file." );
	}

	m_pLoadingScreen->Show( false );
}

void Game::SetState( GameState newState )
{
	m_State = newState;

	using namespace Genesis;
	std::string playlistName;

	if ( m_State == GameState::LoadResources )
	{
		LoadResourcesAsync();
		playlistName = "data/playlists/menu.m3u";
	}
	else if ( m_State == GameState::Menu )
	{
		m_KillSave = false;
		playlistName = "data/playlists/menu.m3u";

		if (m_pMainMenu == nullptr)
		{
			ShowCursor(true);

			if (m_pIntelWindow == nullptr)
			{
				m_pIntelWindow = new IntelWindow();
			}

			m_pSystem = std::make_shared<System>("17260877307600676", true);
			m_pExplorationViewer->View(m_pSystem);
            m_pSystemViewer->View(m_pSystem);

			m_pMainMenu = new MainMenu();

			//ShipCustomisationData data;
			//data.m_CaptainName = "TestCaptain";
			//data.m_pModuleInfoHexGrid = nullptr;
			//data.m_ShipName = "TestShip";
			//StartNewGame(data);
		}
	}
	else if ( m_State == GameState::GalaxyView ) 
	{
		playlistName = "data/playlists/galaxyview.m3u";
	}
	else if ( m_State == GameState::HyperscapeView )
	{
		playlistName = "data/playlists/galaxyview.m3u";
	}

	if ( playlistName.empty() == false )
	{
		ResourceSound* pPlaylistResource = FrameWork::GetResourceManager()->GetResource<ResourceSound*>( playlistName );
		FrameWork::GetSoundManager()->SetPlaylist( pPlaylistResource );
	}

	if ( m_State == GameState::Intro && m_pVideoElement == nullptr )
	{
        ShowCursor( false );

		m_pVideoElement = new Genesis::Gui::Video();
		m_pVideoElement->SetSize( (int)Configuration::GetScreenWidth(), (int)Configuration::GetScreenHeight() );
		m_pVideoElement->SetColour( 1.0f, 1.0f, 1.0f, 1.0f );
		m_pVideoElement->SetBorderMode( Genesis::Gui::PANEL_BORDER_NONE );
		m_pVideoElement->SetPosition( 0, 0 );
		FrameWork::GetGuiManager()->AddElement( m_pVideoElement );
	}
	else if ( m_pVideoElement != nullptr )
	{
		FrameWork::GetGuiManager()->RemoveElement( m_pVideoElement );
		m_pVideoElement = nullptr;
	}
}

void Game::LoadToState( GameState state )
{
	m_pLoadingScreen->Show( true );
	m_LoadToState = state;
}

Faction* Game::GetFaction( const std::string& name ) const
{
	for ( int i = 0; i < (int)FactionId::Count; ++i )
	{
		if ( m_pFaction[ i ] && m_pFaction[ i ]->GetName() == name )
			return m_pFaction[ i ];
	}
	return nullptr;
}

Faction* Game::GetPlayerFaction() const
{
	return m_pFaction[ (int)FactionId::Player ];
}

FleetWeakPtr Game::GetPlayerFleet() const
{
	Faction* pPlayerFaction = GetPlayerFaction();
	if ( pPlayerFaction != nullptr )
	{
		const FleetList& fleets = pPlayerFaction->GetFleets();
		if ( fleets.empty() == false )
		{
			return fleets.back();
		}
	}
	return FleetWeakPtr();
}

SectorInfo* Game::FindSpawnSector() const
{
	return nullptr;
}

void Game::AddFleetCommandIntel( const std::string& text, ModuleInfo* pModuleInfo /* = nullptr */ )
{
	if ( GetIntelWindow() != nullptr )
	{
		GetIntelWindow()->AddFragment( IntelFragment( GameCharacter::FleetIntelligence, "Fleet Intelligence Officer", text, pModuleInfo ) );
	}
}

// The "canBeQueued" parameter is meant to be used for intel that is only relevant if displayed at this very moment.
// If "canBeQueued" is false but the queue already has intel in it, the fragment will be discarded.
void Game::AddIntel( GameCharacter character, const std::string& text, bool canBeQueued /* = true */ )
{
	IntelWindow* pIntelWindow = GetIntelWindow();
	if ( pIntelWindow != nullptr )
	{
		static const std::string characterNames[ static_cast<unsigned int>( GameCharacter::Count ) ] =
		{
			"Fleet Intelligence Officer"
		};

		pIntelWindow->AddFragment( IntelFragment( character, characterNames[ static_cast<unsigned int>( character ) ], text, nullptr, canBeQueued ) );
	}
}

void Game::LoadResourcesAsync()
{
	m_pLoadingScreen->Show( true );
	m_LoaderThread = std::thread( &Game::LoaderThreadMain, this );
}

void Game::LoaderThreadMain()
{
	size_t currentProgress = 0;
	size_t maximumProgress = 0;

	Genesis::Window* pWindow = Genesis::FrameWork::GetWindow();
	SDL_GL_MakeCurrent( pWindow->GetSDLWindow(), pWindow->GetSDLThreadGLContext() );

	using namespace Genesis;
	ResourceManager* pResourceManager = FrameWork::GetResourceManager();
	std::vector<std::string> filesToLoad;
	filesToLoad.reserve( 512 );
	std::filesystem::path dataPath( "data" );
	for ( const auto& entry : std::filesystem::recursive_directory_iterator( dataPath ) )
	{
		const std::string entryPath = ToString( entry.path().c_str() );
		if ( entry.is_regular_file() && pResourceManager->CanLoadResource( entryPath ) )
		{
			filesToLoad.push_back( entryPath );
		}
	}
	maximumProgress += filesToLoad.size();

	for ( const auto& entryPath : filesToLoad )
	{
		pResourceManager->GetResource( entryPath );
		m_pLoadingScreen->SetProgress( ++currentProgress, maximumProgress );
	}

	// Ensure all GPU commands are complete so our render context can use the resources
	// this thread has loaded.
	const GLsync fenceId = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
	const GLuint64 timeout = 5000000000; // 5 second timeout
	while( true )
	{
		GLenum result = glClientWaitSync( fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, timeout );
		if ( result == GL_WAIT_FAILED )
		{
			Genesis::Core::Log::Error() << "glClientWaitSync failed: GL_WAIT_FAILED.";
			exit( -1 );
		}
		else if ( result != GL_TIMEOUT_EXPIRED )
		{
			break;
		}
	}

	Genesis::Core::Log::Info() << "All resources loaded.";

	m_AllResourcesLoaded = true;
	SDL_GL_MakeCurrent( pWindow->GetSDLWindow(), nullptr );
}

bool Game::IsDevelopmentModeActive() const
{
	return true;
}

void Game::RaiseInteractiveWarning( const std::string& text ) const
{
	Popup* pPopup = GetPopup();
	if ( pPopup != nullptr )
	{
		pPopup->Show( PopupMode::Ok, text );
	}

	Genesis::Core::Log::Warning() << text;
}

void Game::SetCursorType( CursorType type )
{
    using namespace Genesis;
    ResourceImage* pCursorTexture = nullptr;
    if ( type == CursorType::Pointer )
    {
        pCursorTexture = (ResourceImage*)FrameWork::GetResourceManager()->GetResource( "data/ui/cursor.png" );
    }
    else if ( type == CursorType::Crosshair )
    {
        pCursorTexture = (ResourceImage*)FrameWork::GetResourceManager()->GetResource( "data/ui/crosshair.png" );
    }

    FrameWork::GetGuiManager()->GetCursor()->SetTexture( pCursorTexture );
    m_CursorType = type;
}

void Game::ShowCursor( bool state )
{
	Genesis::FrameWork::GetGuiManager()->GetCursor()->Show( state );
}

void Game::Pause()
{
	m_IsPaused = true;

	if ( GetPhysicsSimulation() != nullptr )
	{
		GetPhysicsSimulation()->Pause( true );
	}
}

void Game::Unpause()
{
	m_IsPaused = false;

	if ( GetPhysicsSimulation() != nullptr )
	{
		GetPhysicsSimulation()->Pause( false );
	}
}

BlackboardSharedPtr Game::GetBlackboard() 
{
	return m_pBlackboard;
}

Sector* Game::GetCurrentSector() const
{
    return m_pSystem ? m_pSystem->GetCurrentSector() : nullptr;
}


//-------------------------------------------------------------------
// Main
//-------------------------------------------------------------------

int Main(Genesis::CommandLineParameters* parameters) 
{
	using namespace Genesis;
	FrameWork::Initialize();

	FrameWork::CreateWindowGL(
		"Hyperscape", 
		Configuration::GetScreenWidth(),
		Configuration::GetScreenHeight(), 
		Configuration::GetMultiSampleSamples() );

	TaskManager* taskManager = FrameWork::GetTaskManager();

	g_pGame = new Game();
	g_pGame->Initialise();

	while ( taskManager->IsRunning() && g_pGame->IsQuitRequested() == false ) 
	{
		taskManager->Update();
	}

	Configuration::Save();

	delete g_pGame;
	delete parameters;
	FrameWork::Shutdown();

	return 0;
}

}


//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	return Hyperscape::Main(Genesis::FrameWork::CreateCommandLineParameters( (const char**)argv, (int)argc ) );
}
