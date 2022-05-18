// Copyright 2015 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#if USE_FMOD

#include <algorithm>
#include <chrono>
#include <random>

#include <fmod.h>
#include <fmod.hpp>
#include <fmod_errors.h>

#include "configuration.h"
#include "genesis.h"
#include "logger.h"
#include "resources/resourcesound.h"

#include "sound/private/fmod/misc.h"
#include "sound/private/fmod/soundinstance.h"
#include "sound/private/fmod/soundmanager.h"

namespace Genesis::Sound::Private::FMOD
{

///////////////////////////////////////////////////////////////////////////////
// SoundInstance
///////////////////////////////////////////////////////////////////////////////

SoundInstance::SoundInstance()
    : m_pResource( nullptr )
    , m_pChannel( nullptr )
    , m_PlaybackPending( false )
{
}

SoundInstance::~SoundInstance()
{
    if ( IsPlaying() )
    {
        Stop();
    }
}

void SoundInstance::Initialise( ResourceSound* pResourceSound, void* pData )
{
    m_pResource = pResourceSound;
    m_pChannel = reinterpret_cast<::FMOD::Channel*>( pData );

    // By default, a 3D sound is placed by FMOD at the same position as the listener, which is almost certainly
    // not where we want it to be. As such, 3D sounds start paused and are resumed when Set3DAttributes is called.
    m_PlaybackPending = pResourceSound->Is3D();

	// Calling SetVolume when the sound is initialized ensures that the user's volume levels are respected.
	SetVolume( 1.0f );
}

void SoundInstance::Set3DAttributes( const glm::vec3* pPosition /* = nullptr */, const glm::vec3* pVelocity /* = nullptr */ )
{
    if ( m_pResource == nullptr )
    {
        FrameWork::GetLogger()->LogInfo( "Can't call SoundInstance::Set3DAttributes, no resource set, sound not initialised?" );
        return;
    }
    else if ( m_pResource->Is3D() == false )
    {
        FrameWork::GetLogger()->LogInfo( "Can't call Set3DAttributes on sound '%s', resource not set as 3D.", m_pResource->GetFilename().GetFullPath().c_str() );
        return;
    }

    FMOD_VECTOR fmodPosition = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR fmodVelocity = { 0.0f, 0.0f, 0.0f };

    if ( pPosition != nullptr )
        ToFMODVector( *pPosition, &fmodPosition );
    if ( pVelocity != nullptr )
        ToFMODVector( *pVelocity, &fmodVelocity );

    FMOD_RESULT result = m_pChannel->set3DAttributes( pPosition ? &fmodPosition : nullptr, pVelocity ? &fmodVelocity : nullptr );
    CheckFMODResult( result );

    if ( m_PlaybackPending && m_pChannel != nullptr )
    {
        m_pChannel->setPaused( false );
    }
}

void SoundInstance::Get3DAttributes( glm::vec3* pPosition /* = nullptr */, glm::vec3* pVelocity /* = nullptr */ )
{
    FMOD_VECTOR position = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
    m_pChannel->get3DAttributes( pPosition ? &position : nullptr, pVelocity ? &velocity : nullptr );

    if ( pPosition != nullptr )
    {
        pPosition->x = position.x;
        pPosition->y = position.y;
        pPosition->z = position.z;
    }

    if ( pVelocity != nullptr )
    {
        pVelocity->x = velocity.x;
        pVelocity->y = velocity.y;
        pVelocity->z = velocity.z;
    }
}

void SoundInstance::SetMinimumDistance( float value )
{
    if ( m_pResource == nullptr )
    {
        FrameWork::GetLogger()->LogInfo( "Can't call SoundInstance::SetMinimumDistance, no resource set, sound not initialised?" );
        return;
    }
    else if ( m_pResource->Is3D() == false )
    {
        FrameWork::GetLogger()->LogInfo( "Can't call SoundInstance::SetMinimumDistance on sound '%s', resource not set as 3D.", m_pResource->GetFilename().GetFullPath().c_str() );
        return;
    }

    FMOD_RESULT result = m_pChannel->set3DMinMaxDistance( value, 10000.0f );
    CheckFMODResult( result );
}

bool SoundInstance::IsPlaying() const
{
    bool isPlaying = false;

    if ( m_pChannel != nullptr )
    {
        m_pChannel->isPlaying( &isPlaying );
    }

    return isPlaying;
}

void SoundInstance::Stop()
{
    if ( IsPlaying() )
    {
        m_pChannel->stop();
    }
}

ResourceSound* SoundInstance::GetResource() const
{
    return m_pResource;
}

unsigned int SoundInstance::GetLength() const
{
    if ( m_pResource )
    {
        return 0; // TODO: This needs to come from the base Sound, which the SoundInstance doesn't have access to right now.
    }
    else
    {
        return 0;
    }
}

unsigned int SoundInstance::GetPosition() const
{
    if ( m_pChannel )
    {
        unsigned int position;
        m_pChannel->getPosition( &position, FMOD_TIMEUNIT_MS );
        return position;
    }
    else
    {
        return 0;
    }
}

float SoundInstance::GetAudability() const
{
    if ( m_pChannel && IsPlaying() )
    {
        float audability = 0.0f;
        CheckFMODResult( m_pChannel->getAudibility( &audability ) );
        return audability;
    }
    else
    {
        return 0.0f;
    }
}

void SoundInstance::SetVolume( float value )
{
    if ( m_pChannel )
    {
		// Values from Configuration are [0u-100u], need to be remapped to [0.0f-1.0f]
		value *= static_cast<float>( Configuration::GetMasterVolume() ) / 100.0f;
		if ( m_pResource->IsEffect() )
		{
			value *= static_cast<float>( Configuration::GetSFXVolume() ) / 100.0f;
		}
		else
		{
			value *= static_cast<float>( Configuration::GetMusicVolume() ) / 100.0f;
		}

        m_pChannel->setVolume( value );
    }
}

float SoundInstance::GetVolume() const
{
    float volume = 0.0f;
    if ( IsPlaying() )
    {
        CheckFMODResult( m_pChannel->getVolume( &volume ) );
    }
    return volume;
}

} // namespace Genesis::Sound::Private::FMOD

#endif // USE_FMOD
