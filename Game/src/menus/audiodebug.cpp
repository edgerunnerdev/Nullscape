// Copyright 2014 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#include <sstream>
#include <iomanip>

#include <genesis.h>
#include <timer.h>
#include <logger.h>
#include <gui/gui.h>
#include <resources/resourcesound.h>
#include <sound/soundinstance.h>
#include <sound/soundmanager.h>
#include <configuration.h>

#include "menus/audiodebug.h"
#include "hexterminate.h"

namespace Hexterminate
{

AudioDebug::AudioDebug()
{
	using namespace Genesis;

	unsigned int consoleHeight =  (unsigned int)((float)Configuration::GetScreenHeight() * 0.8f) - 16 - 128;
	unsigned int consoleWidth = Configuration::GetScreenWidth() - 832;

	m_pWindow = new EvaWindow( 824, 128, consoleWidth, consoleHeight, true );
	m_pWindow->GetMainPanel()->Show( false );
	m_pWindow->GetMainPanel()->SetColour( 0.0f, 0.0f, 0.0f, 0.9f );

	m_pTextGui = new Genesis::Gui::Text();
	m_pTextGui->SetFont( EVA_FONT );
	m_pTextGui->SetColour( EVA_TEXT_COLOUR );
	m_pTextGui->SetSize( 800.0f - 32.0f, (float)consoleHeight - 16.0f );
	m_pTextGui->SetPosition( 16.0f, 16.0f );
	m_pWindow->GetMainPanel()->AddElement( m_pTextGui );
}

AudioDebug::~AudioDebug()
{
    delete m_pWindow;
}

bool AudioDebug::IsVisible() const
{
	return m_pWindow->GetMainPanel()->IsVisible();
}

void AudioDebug::Update( float delta )
{
	if ( IsVisible() == false )
		return;

	using namespace Genesis;
	std::stringstream ss;
	Sound::SoundManager* pSoundManager = FrameWork::GetSoundManager();

	ss << "Active sounds: " << pSoundManager->GetActiveSoundCount() << std::endl;

	ResourceSound* pPlaylistResource = pSoundManager->GetPlaylistResource();
	std::string playlistName = pPlaylistResource ? pPlaylistResource->GetFilename().GetName() : std::string("none");

	if ( pPlaylistResource )
	{
		ss << "Current playlist: '" << pPlaylistResource->GetFilename().GetName() << "'" << std::endl;
	}
	else
	{
		ss << "No playlist active" << std::endl;
	}

	Sound::SoundInstanceSharedPtr currentSong = pSoundManager->GetCurrentSong();
	if ( currentSong )
	{
		ss << "Current song: " << currentSong->GetResource()->GetFilename().GetName() << " (";

		// Display the current position and length of the song.
		// Force the seconds to be displayed always as two digits
		unsigned int position = currentSong->GetPosition();
		ss << ( position / 1000 / 60 ) << ":" << std::setw(2) << std::setfill('0') << ( position / 1000 % 60 );
		ss << " / ";

		unsigned int length = currentSong->GetLength();
		ss << ( length / 1000 / 60 ) << ":" << ( length / 1000 % 60 );
		ss << ")";
	}

	ss << std::endl << std::endl;
	ss << "Ref  Type  Aud     Dst   File" << std::endl;

	for ( auto& pSoundInstance : pSoundManager->GetSoundInstances() )
	{
		long refCount = pSoundInstance.use_count() - 1; // Not counting the reference created by having to print this information
		ss << std::setw(3) << std::setfill(' ') << refCount << "  "; 
		
		if ( pSoundInstance->GetResource()->IsHardwareMixed() )
			ss << "HARD  ";
		else if ( pSoundInstance->GetResource()->IsSoftwareMixed() )
			ss << "SOFT  ";

		ss << std::setw(6) << std::setfill(' ') << std::setprecision(4) << pSoundInstance->GetAudability() << " ";

		int distance = 0;
		if ( pSoundInstance->GetResource()->Is3D() )
		{
			glm::vec3 listenerPosition = pSoundManager->GetListenerPosition();
			glm::vec3 soundPosition;
			pSoundInstance->Get3DAttributes( &soundPosition );
			glm::vec3 diff = listenerPosition - soundPosition;
			distance = static_cast<int>(diff.length());
			ss << std::setw(9) << std::setfill(' ') << distance << "m  ";
		}
		else
		{
			ss << std::setw(9) << std::setfill(' ') << "-  ";
		}

		ss << pSoundInstance->GetResource()->GetFilename().GetName() << std::endl;
	}

	m_pTextGui->SetText( ss.str() );

	m_pWindow->Update( delta );
}

void AudioDebug::Show( bool state )
{
	m_pWindow->GetMainPanel()->Show( state );
}

}