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

#include <filesystem>
#include <list>
#include <memory>
#include <string>
#include <taskmanager.h>
#include <thread>
#include <vector>

#if USE_STEAM
#include "beginexternalheaders.h"
#include "endexternalheaders.h"
#include "steam/steam_api.h"
#endif

#include "blackboard.h"
#include "faction/faction.h"
#include "menus/cursortype.h"
#include "menus/musictitle.h"
#include "ship/ship.h"

static const std::string HYPERSCAPE_BUILD("0.0.1");

namespace Genesis
{

struct Material;
class ResourceSound;
class ModelViewer;
using ModelViewerUniquePtr = std::unique_ptr<ModelViewer>;

namespace Gui
{
class Text;
class Video;
} // namespace Gui

namespace Physics
{
class Simulation;
}

} // namespace Genesis

namespace Hyperscape
{

namespace UI
{
class Editor;
using EditorUniquePtr = std::unique_ptr<Editor>;
class RootElement;
using RootElementUniquePtr = std::unique_ptr<RootElement>;
} // namespace UI

class Sector;
class Game;
class ModuleInfoManager;
class ModuleInfo;
class ShipInfoManager;
class MainMenu;
class Console;
class AudioDebug;
class Hotbar;
class IntelWindow;
class Galaxy;
class Fleet;
class HyperspaceMenu;
class Popup;
class MusicTitle;
class TutorialWindow;
class RequestManager;
class Perks;
class LoadingScreen;
class AchievementsManager;
class SaveGameStorage;
class ShipOutline;

GENESIS_DECLARE_SMART_PTR(EntityViewer);
GENESIS_DECLARE_SMART_PTR(Player);
GENESIS_DECLARE_SMART_PTR(System);
GENESIS_DECLARE_SMART_PTR(SystemViewer);
GENESIS_DECLARE_SMART_PTR(SaveGameHeader);

extern Game* g_pGame;

using StringVector = std::vector<std::string>;
using WStringVector = std::vector<std::wstring>;
using LoadingScreenUniquePtr = std::unique_ptr<LoadingScreen>;
using ShipOutlineUniquePtr = std::unique_ptr<ShipOutline>;

struct NewGameParameters;

enum class GameCharacter
{
    FleetIntelligence,
    Count
};


///////////////////////////////////////////////////////////////////////////
// Game
///////////////////////////////////////////////////////////////////////////

enum class GameState
{
    Intro = 0,
    LoadResources,
    Menu,
    GalaxyView,
    HyperscapeView,
    Shipyard,
    Combat,
    Unknown
};

class Game : public Genesis::Task
{
public:
    Game();
    ~Game();
    void Initialise();
    Genesis::TaskStatus Update(float delta);

    Genesis::Physics::Simulation* GetPhysicsSimulation() const;
    Sector* GetCurrentSector() const;
    ModuleInfoManager* GetModuleInfoManager() const;
    ShipInfoManager* GetShipInfoManager() const;
    Faction* GetFaction(const std::string& name) const;
    Faction* GetFaction(FactionId faction) const;
    Faction* GetPlayerFaction() const;
    PlayerSharedPtr GetPlayer() const;
    FleetWeakPtr GetPlayerFleet() const;
    Galaxy* GetGalaxy() const;
    IntelWindow* GetIntelWindow() const;
    Popup* GetPopup() const;
    SectorInfo* FindSpawnSector() const;
    AchievementsManager* GetAchievementsManager() const;
    SaveGameStorage* GetSaveGameStorage() const;
    ShipOutline* GetShipOutline() const;

    void RaiseInteractiveWarning(const std::string& text) const;
    void SetCursorType(CursorType type);
    CursorType GetCursorType() const;
    void ShowCursor(bool state);

    void StartNewGame(const ShipCustomisationData& customisationData);
    void EndGame();
    void KillSaveGame();
    bool SaveGame();
    void LoadGame(SaveGameHeaderWeakPtr pSaveGameHeader);
    void LoadToState(GameState state);
    void SetState(GameState state);
    GameState GetState() const;

    void AddFleetCommandIntel(const std::string& text, ModuleInfo* pModuleInfo = nullptr);
    void AddIntel(GameCharacter character, const std::string& text, bool canBeQueued = true);
    float GetPlayedTime() const;
    void SetPlayedTime(float time);

    bool IsDevelopmentModeActive() const;

    void Pause();
    void Unpause();
    bool IsPaused() const;

    void SetInputBlocked(bool state);
    bool IsInputBlocked() const;

    void Quit();
    bool IsQuitRequested() const;

    BlackboardSharedPtr GetBlackboard();

    UI::RootElement* GetUIRoot() const;

private:
    void LoadResourcesAsync();
    void LoaderThreadMain();
    void EndGameAux();
    void LoadGameAux();
    void ToggleImGui();

    MainMenu* m_pMainMenu;
    Console* m_pConsole;
    AudioDebug* m_pAudioDebug;
    ModuleInfoManager* m_pModuleInfoManager;
    ShipInfoManager* m_pShipInfoManager;
    PlayerSharedPtr m_pPlayer;
    SystemSharedPtr m_pSystem;
    SystemViewerUniquePtr m_pSystemViewer;
    MusicTitle* m_pMusicTitle;
    TutorialWindow* m_pTutorialWindow;
    GameState m_State;
    IntelWindow* m_pIntelWindow;
    LoadingScreenUniquePtr m_pLoadingScreen;
    AchievementsManager* m_pAchievementsManager;
    ShipOutlineUniquePtr m_pShipOutline;

    Faction* m_pFaction[static_cast<unsigned int>(FactionId::Count)];
    float m_PlayedTime;

    Popup* m_pPopup;

    bool m_IsPaused;
    bool m_IsInputBlocked;
    float m_InputBlockedTimer;

    Genesis::Gui::Text* m_pFrameText;
    bool m_ContextualTipsEnabled;
    bool m_QuitRequested;
    Genesis::Gui::Video* m_pVideoElement;
    CursorType m_CursorType;
    GameState m_LoadToState;
    std::filesystem::path m_GameToLoad;

    Genesis::InputCallbackToken m_ImGuiToggleToken;
    Genesis::Physics::Simulation* m_pPhysicsSimulation;

    std::unique_ptr<SaveGameStorage> m_pSaveGameStorage;
    bool m_KillSave;

    UI::RootElementUniquePtr m_pUIRootElement;
    UI::EditorUniquePtr m_pUIEditor;
    Genesis::ModelViewerUniquePtr m_pModelViewer;
    bool m_ShowImguiTestWindow;

    std::thread m_LoaderThread;
    std::atomic_bool m_AllResourcesLoaded;

    BlackboardSharedPtr m_pBlackboard;
    EntityViewerUniquePtr m_pEntityViewer;
};

inline Genesis::Physics::Simulation* Game::GetPhysicsSimulation() const
{
    return m_pPhysicsSimulation;
}

inline ModuleInfoManager* Game::GetModuleInfoManager() const
{
    return m_pModuleInfoManager;
}

inline ShipInfoManager* Game::GetShipInfoManager() const
{
    return m_pShipInfoManager;
}

inline AchievementsManager* Game::GetAchievementsManager() const
{
    return m_pAchievementsManager;
}

inline ShipOutline* Game::GetShipOutline() const
{
    return m_pShipOutline.get();
}

inline PlayerSharedPtr Game::GetPlayer() const
{
    return m_pPlayer;
}

inline Galaxy* Game::GetGalaxy() const
{
    return nullptr;
}

inline Faction* Game::GetFaction(FactionId id) const
{
    return m_pFaction[static_cast<int>(id)];
}

inline GameState Game::GetState() const
{
    return m_State;
}

inline IntelWindow* Game::GetIntelWindow() const
{
    return m_pIntelWindow;
}

inline float Game::GetPlayedTime() const
{
    return m_PlayedTime;
}

inline void Game::SetPlayedTime(float time)
{
    m_PlayedTime = time;
}

inline Popup* Game::GetPopup() const
{
    return m_pPopup;
}

inline bool Game::IsPaused() const
{
    return m_IsPaused;
}

inline void Game::Quit()
{
    m_QuitRequested = true;
}

inline bool Game::IsQuitRequested() const
{
    return m_QuitRequested;
}

inline CursorType Game::GetCursorType() const
{
    return m_CursorType;
}

inline UI::RootElement* Game::GetUIRoot() const
{
    return m_pUIRootElement.get();
}

} // namespace Hyperscape
