// Copyright 2016 Pedro Nunes
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

#include <sstream>

#include <genesis.h>
#include <configuration.h>
#include <rendersystem.h>

#include "menus/intelwindow.h"
#include "menus/lootwindow.h"
#include "menus/eva.h"
#include "menus/meter.h"
#include "ship/inventory.h"
#include "ship/ship.h"
#include "ship/moduleinfo.h"
#include "game.hpp"
#include "player.h"
#include "stringaux.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// LootWindow
///////////////////////////////////////////////////////////////////////////////

LootWindow::LootWindow() :
m_pMainPanel( nullptr ),
m_pModuleName( nullptr ),
m_pIcon( nullptr ),
m_pWingsLeft( nullptr ),
m_pWingsRight( nullptr ),
m_pPerkMeter( nullptr ),
m_pPerkAcquired( nullptr ),
m_Dirty( false ),
m_PanelWidth( 400.0f ),
m_PanelHeight( 400.0f ),
m_IconSize( 64.0f ),
m_DisplayTimer( 0.0f ),
m_pPerkPointAcquiredSFX( nullptr ),
m_pPerkProgressSFX( nullptr )
{
	using namespace Genesis;

	ResourceManager* pRm = FrameWork::GetResourceManager();
	ResourceImage* pLootIcon = (ResourceImage*)pRm->GetResource( "data/ui/icons/armour.png" );

	Gui::GuiManager* pGuiManager = Genesis::FrameWork::GetGuiManager();

	m_pMainPanel = new Gui::Panel();
	m_pMainPanel->SetSize( m_PanelWidth, m_PanelHeight );
	m_pMainPanel->SetColour( 0.0f, 0.0f, 0.0f, 0.0f );
	m_pMainPanel->SetBorderMode( Gui::PANEL_BORDER_NONE );
    m_pMainPanel->SetPosition( ( Configuration::GetScreenWidth() - m_PanelWidth ) / 2.0f, 8.0f );
	m_pMainPanel->SetHiddenForCapture( true );
	pGuiManager->AddElement( m_pMainPanel );

	ResourceShader* pIconShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/gui_loot.glsl");
	ResourceShader* pWingsShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/gui_wings.glsl");

	m_pIcon = new Gui::Image();
    m_pIcon->SetPosition( ( m_PanelWidth - m_IconSize ) / 2.0f, 96.0f );
    m_pIcon->SetSize( m_IconSize, m_IconSize );
	m_pIcon->SetTexture( nullptr );
	m_pIcon->SetColour( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pIcon->SetShader( pIconShader );
	m_pIcon->SetTexture( pLootIcon );
	m_pMainPanel->AddElement( m_pIcon );

    ResourceImage* pWingsLeft = (ResourceImage*)pRm->GetResource( "data/ui/wings_left.png" );
	m_pWingsLeft = new Gui::Image();
	m_pWingsLeft->SetPosition( 0.0f, 0.0f );
    m_pWingsLeft->SetSize( (int)pWingsLeft->GetWidth(), (int)pWingsLeft->GetHeight() );
	m_pWingsLeft->SetTexture( pWingsLeft );
	m_pWingsLeft->SetColour( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pWingsLeft->SetShader( pWingsShader );
	m_pMainPanel->AddElement( m_pWingsLeft );

    ResourceImage* pWingsRight = (ResourceImage*)pRm->GetResource( "data/ui/wings_right.png" );
    m_pWingsRight = new Gui::Image();
    m_pWingsRight->SetPosition( m_PanelWidth - pWingsRight->GetWidth(), 0.0f );
    m_pWingsRight->SetSize( (int)pWingsRight->GetWidth(), (int)pWingsRight->GetHeight() );
	m_pWingsRight->SetTexture( pWingsRight );
	m_pWingsRight->SetColour( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pWingsRight->SetShader( pWingsShader );
	m_pMainPanel->AddElement( m_pWingsRight );

	m_pModuleName = new Gui::Text();
    m_pModuleName->SetSize( m_PanelWidth, 32.0f );
	m_pModuleName->SetColour( EVA_TEXT_COLOUR );
	m_pModuleName->SetFont( EVA_FONT );
	m_pMainPanel->AddElement( m_pModuleName );

	m_pPerkMeter = new Meter();
	m_pPerkMeter->Init( 0, 10 );
	m_pPerkMeter->SetPosition( floorf( ( m_PanelWidth - m_pPerkMeter->GetWidth() ) / 2.0f ), 200.0f );
	m_pPerkMeter->SetBorderColour( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pMainPanel->AddElement( m_pPerkMeter );

	m_pPerkAcquired = new Gui::Text();
	m_pPerkAcquired->SetPosition( 0, 220 );
    m_pPerkAcquired->SetSize( m_PanelWidth, 32.0f );
	m_pPerkAcquired->SetColour( EVA_TEXT_COLOUR );
	m_pPerkAcquired->SetFont( EVA_FONT );
	m_pMainPanel->AddElement( m_pPerkAcquired );

    InitialiseSounds();
}

LootWindow::~LootWindow()
{
	Genesis::Gui::GuiManager* pGuiManager = Genesis::FrameWork::GetGuiManager();
	pGuiManager->RemoveElement( m_pMainPanel );
}

void LootWindow::InitialiseSounds()
{
    m_pPerkProgressSFX = Genesis::Gui::LoadSFX( "data/sfx/perk_part_acquired.wav" );
    m_pPerkPointAcquiredSFX = Genesis::Gui::LoadSFX( "data/sfx/perk_point_acquired.wav" );
}

void LootWindow::Update( float delta )
{
	static const float sMaximumDisplayTime = 5.0f;

	const bool show = ( m_LootQueue.empty() == false && m_DisplayTimer < sMaximumDisplayTime );
	m_pMainPanel->Show( show );

	if ( m_LootQueue.empty() == false )
	{
		m_DisplayTimer += delta;

		// Positions the loot window in relationship to the intel window, if present
		float yOffset = 8.0f;
		IntelWindow* pIntelWindow = g_pGame->GetIntelWindow();
		if ( pIntelWindow != nullptr && pIntelWindow->IsVisible() )
		{
			yOffset = pIntelWindow->GetPosition().y + pIntelWindow->GetSize().y + 8.0f;
		}
		m_pMainPanel->SetPosition( ( Genesis::Configuration::GetScreenWidth() - m_PanelWidth ) / 2.0f, yOffset );

		// Sets the colours & sizes of the loot window elements.
		// Colour is based on the module's rarity.
        const float alpha = CalculateAlpha();
		const float iconScalingAmount = CalculateIconScaling();
		const float iconSize = m_IconSize * iconScalingAmount;
		m_pIcon->SetSize( iconSize, iconSize );
		m_pIcon->SetPosition( ( m_PanelWidth - iconSize ) / 2.0f, 128.0f - iconSize / 2.0f );
		m_pIcon->SetColour( Genesis::Colour( 1.0f, 1.0f, 1.0f, alpha ) );
        m_pPerkMeter->SetColour( 0.0f, 0.0f, 0.0f, 0.5f * alpha );
        m_pPerkMeter->SetBorderColour( 1.0f, 1.0f, 1.0f, 0.5f * alpha );
        m_pPerkMeter->SetPipColour( 1.0f, 1.0f, 1.0f, 0.75f * alpha );

		Genesis::Colour highlightColour = ModuleRarityToColour( m_LootQueue.front().pModuleInfo->GetRarity() );
		highlightColour.a = CalculateAlpha();
		m_pModuleName->SetColour( highlightColour );
		m_pWingsLeft->SetColour( highlightColour );
		m_pWingsRight->SetColour( highlightColour );
		m_pPerkAcquired->SetColour( highlightColour );

		if ( m_DisplayTimer >= sMaximumDisplayTime )
		{
			m_LootQueue.pop_front();
			m_DisplayTimer = 0.0f;
			m_Dirty = !m_LootQueue.empty();
		}
	}

    if ( m_Dirty )
    {
		SDL_assert( m_LootQueue.empty() == false );
		const LootElement& loot = m_LootQueue.front();

		std::stringstream lootText;
		if ( loot.quantity > 1 )
		{
			lootText << loot.quantity << "x " << loot.pModuleInfo->GetFullName(); 
		}
		else
		{
			lootText << loot.pModuleInfo->GetFullName();
		}

		m_pModuleName->SetText( lootText.str() );
		Genesis::Colour textColour = ModuleRarityToColour( loot.pModuleInfo->GetRarity() );
		textColour.a = CalculateAlpha();
		m_pModuleName->SetColour( textColour );

		// Centers the name of the module
		const float textWidth = m_pModuleName->GetFont()->GetTextLength( lootText.str() );
		m_pModuleName->SetPosition( ( m_PanelWidth - textWidth ) / 2.0f, 168.0f );

		m_Dirty = false;
    }

//#ifndef _FINAL
//    Test( delta );
//#endif
}

void LootWindow::HandleGameEvent( GameEvent* pEvent )
{

}

// This test function gives the player a module every few seconds. 
// Quantity is randomised to cover the case of armour module drops, which drop in multiples.
void LootWindow::Test( float delta )
{

}

float LootWindow::CalculateIconScaling() const
{
	const float t = std::min( m_DisplayTimer * 2.0f, 1.0f );
	return - ( t * t ) + 2.0f;
}

float LootWindow::CalculateAlpha() const
{
	const float t = std::min( m_DisplayTimer * 2.0f, 1.0f );
	return t * t;
}

}