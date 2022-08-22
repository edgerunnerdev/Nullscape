// Copyright 2021 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#include <functional>
#include <sstream>

#include <log.hpp>
#include <genesis.h>

#include "menus/popup.h"
#include "menus/newgamewindow.h"
#include "ship/shipinfo.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "ui/image.h"
#include "ui/inputarea.h"
#include "ui/meter.h"
#include "ui/panel.h"
#include "ui/radiobutton.h"
#include "ui/text.h"
#include "ui/togglegroup.h"
#include "game.hpp"
#include "savegamestorage.h"

namespace Nullscape
{

NewGameWindow::NewGameWindow() : UI::Window( "New game window" )
{
	using namespace std::placeholders;

	m_Pages.resize( static_cast<size_t>( PageId::Count ) );
	m_CurrentPage = PageId::Count;

	m_pButtonNext = std::make_shared<UI::Button>( "Next button", std::bind( &NewGameWindow::OnPageSwitchButtonPressed, this, _1 ) );
	m_pButtonPrevious = std::make_shared<UI::Button>( "Previous button", std::bind( &NewGameWindow::OnPageSwitchButtonPressed, this, _1 ) );
	GetContentPanel()->Add( m_pButtonNext );
	GetContentPanel()->Add( m_pButtonPrevious );

	CreateGameModeSelectionPage();
	CreateShipSelectionPage();
	CreateCustomisationPage();
	CreateFactionPresencePage();

	for ( auto& pPage : m_Pages )
	{
		if ( pPage != nullptr )
		{
			GetContentPanel()->Add( pPage );
		}
	}

	Select( PageId::GameMode );
}

void NewGameWindow::Update()
{
	UI::Window::Update();
}

void NewGameWindow::Reset()
{
	UI::Window::Reset();
}

void NewGameWindow::Select( PageId pageId )
{

}

void NewGameWindow::StartNewGame()
{

}

void NewGameWindow::CreateGameModeSelectionPage()
{

}

void NewGameWindow::CreateShipSelectionPage()
{

}

void NewGameWindow::CreateCustomisationPage()
{

}

void NewGameWindow::CreateFactionPresencePage()
{

}

UI::ElementSharedPtr NewGameWindow::CreateGameModeInfoPanel( const std::string& name ) const
{
	return nullptr;
}

UI::ElementSharedPtr NewGameWindow::CreateShipInfoPanel( const std::string& name ) const
{
	return nullptr;
}

UI::ElementSharedPtr NewGameWindow::CreateShipDetailsPanel()
{
	return nullptr;
}

UI::ElementSharedPtr NewGameWindow::CreateDifficultyPanel()
{
	UI::PanelSharedPtr pPanel = std::make_shared<UI::Panel>( "Difficulty panel" );
	return pPanel;
}

UI::ElementSharedPtr NewGameWindow::CreatePreferencesPanel()
{
	return nullptr;
}

void NewGameWindow::OnPageSwitchButtonPressed( const std::any& userData )
{
	PageId pageId = std::any_cast<PageId>( userData );
	Select( pageId );
}

void NewGameWindow::OnGameModeButtonPressed( const std::any& userData )
{

}

void NewGameWindow::OnShipButtonPressed( const std::any& userData )
{
	std::string selectedShip = std::any_cast<std::string>( userData );
	const ShipInfo* pInfo = g_pGame->GetShipInfoManager()->Get( g_pGame->GetPlayerFaction(), selectedShip );
	if ( pInfo == nullptr )
	{
        Genesis::Core::Log::Error() << "Couldn't find hexgrid '" << selectedShip << "' for player faction";
	}
	else
	{
		m_ShipCustomisationData.m_pModuleInfoHexGrid = pInfo->GetModuleInfoHexGrid();

		// The player's fleet is composed of the same type of ship that he starts the game with.
		// This helps with keeping the entire fleet using the same style of play.
		m_CompanionShipTemplate = selectedShip;
	}
}

} // namespace Nullscape
