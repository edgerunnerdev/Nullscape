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

#include "menus/fleetwindow.h"

#include <sstream>

#include <genesis.h>
#include <configuration.h>

#include "ship/shipinfo.h"
#include "ui/button.h"
#include "ui/image.h"
#include "ui/panel.h"
#include "ui/text.h"
#include "ui/rootelement.h"
#include "ui/scrollingelement.h"
#include "player.h"
#include "game.hpp"
#include "stringaux.h"


namespace Hyperscape
{

/////////////////////////////////////////////////////////////////////
// FleetWindow
/////////////////////////////////////////////////////////////////////

FleetWindow::FleetWindow() : 
UI::Window( "Fleet window" ),
m_RequisitionTextInitialized( false )
{
	m_pFleetPanel = std::make_shared<UI::Panel>( "Fleet panel" );
	GetContentPanel()->Add( m_pFleetPanel );
	m_pFleetSubtitle = std::make_shared<UI::Text>( "Fleet subtitle" );
	m_pFleetPanel->Add( m_pFleetSubtitle );
	m_pRequisitionShipsPanel = std::make_shared<UI::Panel>( "Requisition ships panel ");
	GetContentPanel()->Add( m_pRequisitionShipsPanel );
	m_pRequisitionShipsSubtitle = std::make_shared<UI::Text>( "Requisition ships subtitle" );
	m_pRequisitionShipsPanel->Add( m_pRequisitionShipsSubtitle );
	m_pRequisitionShipsArea = std::make_shared<UI::ScrollingElement>( "Requisition ships area" );
	m_pRequisitionShipsPanel->Add( m_pRequisitionShipsArea );

	PopulateFleetShips();
	PopulateRequisitionShips();
}

FleetWindow::~FleetWindow()
{

}

void FleetWindow::Show( bool state )
{
	UI::Window::Show( state );
}

void FleetWindow::PopulateFleetShips()
{
	for ( int i = 0; i < 5; ++i )
	{
		std::stringstream panelId;
		panelId << "Ship " << i + 1;

		FleetShipInfo fsi;
		fsi.isPlayerShip = ( i == 0 );
		fsi.pPanel = std::make_shared<UI::Panel>( panelId.str() );
		m_pFleetPanel->Add( fsi.pPanel );
		fsi.pNameText = std::make_shared<UI::Text>( "Name text" );
		fsi.pPanel->Add( fsi.pNameText );
		fsi.pCategoryText = std::make_shared<UI::Text>( "Category text" );
		fsi.pPanel->Add( fsi.pCategoryText );
		fsi.pReturnButton = std::make_shared<UI::Button>( "Return button", []( std::any userData ){} );
		fsi.pPanel->Add( fsi.pReturnButton );
		fsi.pUnavailableIcon = std::make_shared<UI::Image>( "Unavailable icon" );
		fsi.pUnavailableIcon->SetShader("data/shaders/gui_textured.glsl");
		fsi.pUnavailableIcon->SetColour( 1.0f, 0.0f, 0.0f, 1.0f );
		fsi.pPanel->Add( fsi.pUnavailableIcon );
		fsi.pUnavailableText = std::make_shared<UI::Text>( "Unavailable text" );
		fsi.pPanel->Add( fsi.pUnavailableText );

		m_FleetShipInfos.push_back( fsi );
	}
}

void FleetWindow::PopulateRequisitionShips()
{
	const std::string shipNames[] =
	{
		"lancer", 
		"phalanx", 
		"ironclad",
		"t1_battlecruiser_luna",
		"t1_capital_dominion",
		"special_t2_gunship_lancer_mk2",
		"special_t2_battlecruiser_wyvern",
        "special_t2_battlecruiser_glory",
		"special_t2_battlecruiser_keeper",
		"special_t3_battlecruiser_dies_irae",
        "special_t3_battlecruiser_tyrant",
		"special_t3_capital_arbellatris"
	};

	Faction* pImperialFaction = g_pGame->GetFaction( FactionId::Empire );
	for ( auto& shipName : shipNames )
	{
		const ShipInfo* pShipInfo = g_pGame->GetShipInfoManager()->Get( pImperialFaction, shipName );
		if ( pShipInfo != nullptr )
		{
			RequisitionShipInfo rsi;
			rsi.pShipInfo = pShipInfo;
			rsi.pPanel = std::make_shared<UI::Panel>( shipName );
			rsi.pBackground = std::make_shared<UI::Image>( "Background" );
			rsi.pTitle = std::make_shared<UI::Text>( "Title" );

			m_pRequisitionShipsArea->GetScrollingArea()->Add( rsi.pPanel );
			rsi.pPanel->Add( rsi.pBackground );
			rsi.pBackground->Add( rsi.pTitle );

			rsi.pWeaponsIcon = std::make_shared<UI::Image>( "Weapons icon" );
			rsi.pBackground->Add( rsi.pWeaponsIcon );
			rsi.pWeaponsText = std::make_shared<UI::Text>( "Weapons text" );
			rsi.pBackground->Add( rsi.pWeaponsText );
			rsi.pDefenseIcon = std::make_shared<UI::Image>( "Defense icon" );
			rsi.pBackground->Add( rsi.pDefenseIcon );
			rsi.pDefenseText = std::make_shared<UI::Text>( "Defense text" );
			rsi.pBackground->Add( rsi.pDefenseText );
			rsi.pInfluenceIcon = std::make_shared<UI::Image>( "Influence icon" );
			rsi.pInfluenceIcon->SetShader( "data/shaders/gui_textured.glsl" );
			rsi.pBackground->Add( rsi.pInfluenceIcon );
			rsi.pInfluenceText = std::make_shared<UI::Text>( "Influence text" );
			rsi.pBackground->Add( rsi.pInfluenceText );
			rsi.pPerkIcon = std::make_shared<UI::Image>( "Perk icon" );
			rsi.pPerkIcon->SetShader( "data/shaders/gui_textured.glsl" );
			rsi.pBackground->Add( rsi.pPerkIcon );
			rsi.pPerkText = std::make_shared<UI::Text>( "Perk text" );
			rsi.pBackground->Add( rsi.pPerkText );

			rsi.pRequisitionButton = std::make_shared<UI::Button>( "Requisition button", [ this, pShipInfo ]( std::any userData ){ RequisitionShip( pShipInfo ); } );
			rsi.pRequisitionButton->Enable( CanRequisitionShip( pShipInfo ) );
			rsi.pBackground->Add( rsi.pRequisitionButton );

			m_RequisitionShipInfos.push_back( rsi );
		}
	}
}

void FleetWindow::RefreshFleetShips()
{

}

void FleetWindow::RefreshRequisitionShips()
{
	for ( auto& rsi : m_RequisitionShipInfos )
	{
		if ( CanRequisitionShip( rsi.pShipInfo ) )
		{
			rsi.pBackground->SetShader( "data/shaders/gui_textured.glsl" );
			rsi.pRequisitionButton->Enable( true );
		}
		else
		{
			rsi.pBackground->SetShader( "data/shaders/gui_textured_greyscale.glsl" );
			rsi.pRequisitionButton->Enable( false );
		}
	}
}

bool FleetWindow::CanRequisitionShip( const ShipInfo* pShipInfo ) const
{
	return false;
}

bool FleetWindow::HasSufficientInfluence( const ShipInfo* pShipInfo ) const
{
	return false;
}

bool FleetWindow::HasNecessaryPerk( const ShipInfo* pShipInfo ) const
{
	return false;
}

void FleetWindow::RequisitionShip( const ShipInfo* pShipInfo )
{

}

}
