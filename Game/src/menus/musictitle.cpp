// Copyright 2015 Pedro Nunes
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

#include <sstream>
#include <iomanip>
#include <algorithm>

#include <genesis.h>
#include <timer.h>
#include <log.hpp>
#include <gui/gui.h>
#include <resources/resourcesound.h>
#include <configuration.h>
#include <sound/soundinstance.h>
#include <sound/soundmanager.h>

#include "menus/musictitle.h"
#include "game.hpp"

namespace Nullscape
{

MusicTitle::MusicTitle() :
m_pTextGui( nullptr ),
m_pIcon( nullptr )
{
	using namespace Genesis;

	m_pTextGui = new Gui::Text();
	m_pTextGui->SetFont( EVA_FONT );
	m_pTextGui->SetColour( EVA_TEXT_COLOUR );
	m_pTextGui->SetSize( 300.0f, 16.0f );
	m_pTextGui->SetPosition( 40.0f, 12.0f );
	m_pTextGui->SetHiddenForCapture( true );
	FrameWork::GetGuiManager()->AddElement( m_pTextGui );

	m_pIcon = new Gui::Image();
	m_pIcon->SetPosition( 4.0f, 4.0f );
	m_pIcon->SetSize( 32.0f, 32.0f );
	m_pIcon->SetTexture( (ResourceImage*)FrameWork::GetResourceManager()->GetResource( "data/ui/icons/headphones.png" ) );
	m_pIcon->SetColour( 1.0f, 1.0f, 1.0f, 0.0f );
	m_pIcon->SetHiddenForCapture( true );
	FrameWork::GetGuiManager()->AddElement( m_pIcon );
}

MusicTitle::~MusicTitle()
{
	Genesis::FrameWork::GetGuiManager()->RemoveElement( m_pTextGui );
	Genesis::FrameWork::GetGuiManager()->RemoveElement( m_pIcon );
}

void MusicTitle::Update( float delta )
{
	using namespace Genesis;
	Sound::SoundManager* pSoundManager = FrameWork::GetSoundManager();

	Sound::SoundInstanceSharedPtr currentSong = pSoundManager->GetCurrentSong();
	bool isVisible = false;
	if ( currentSong && g_pGame->GetState() != GameState::Menu )
	{
		std::string songName = currentSong->GetResource()->GetFilename().GetName();
		songName = songName.substr( 0, songName.length() - 4 );
		std::stringstream ss;
		ss << "Kontinue - " << songName;
		m_pTextGui->SetText( ss.str() );

		const float p = (float)currentSong->GetPosition() / 1000.0f;
		float alpha = 1.0f;
		if ( p < 1.0f ) alpha = p;
		else if ( p > 4.0f ) alpha = 1.0f - std::min( p - 4.0f, 1.0f );

		m_pTextGui->SetColour( 1.0f, 1.0f, 1.0f, alpha );
		m_pIcon->SetColour( 1.0f, 1.0f, 1.0f, alpha );
		
		isVisible = ( alpha > 0.0f );

		m_pTextGui->SetPosition( 40, g_pGame->GetCurrentSector() ? 12 : 26 );
		m_pIcon->SetPosition( 4, g_pGame->GetCurrentSector() ? 4 : 18 );
	}

	m_pTextGui->Show( isVisible );
	m_pIcon->Show( isVisible );
}

}