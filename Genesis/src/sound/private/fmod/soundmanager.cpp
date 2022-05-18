// Copyright 2021 Pedro Nunes
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
#include <random>

#include <fmod.h>
#include <fmod.hpp>
#include <fmod_errors.h>

#include "resources/resourcesound.h"
#include "sound/private/fmod/misc.h"
#include "sound/private/fmod/soundinstance.h"
#include "sound/private/fmod/soundmanager.h"
#include "sound/soundinstance.h"
#include "genesis.h"
#include "logger.h"

namespace Genesis::Sound::Private::FMOD
{

SoundManager::SoundManager()
    : m_pSystem( nullptr )
    , m_pActivePlaylist( nullptr )
    , m_PlaylistSoundIndex( -1 )
    , m_Timer( 0.0f )
{
    m_Seed = static_cast<unsigned int>( std::chrono::system_clock::now().time_since_epoch().count() );

    FMOD_RESULT result;
    result = ::FMOD::System_Create( &m_pSystem );

    unsigned int version;
    result = m_pSystem->getVersion( &version );
    CheckFMODResult( result );

    if ( version < FMOD_VERSION )
    {
        Genesis::FrameWork::GetLogger()->LogError( "You are using an old version of FMOD %08x. This program requires %08x\n", version, FMOD_VERSION );
        return;
    }

    int numdrivers;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS caps;
    char name[ 256 ];

    result = m_pSystem->getNumDrivers( &numdrivers );
    CheckFMODResult( result );

    if ( numdrivers == 0 )
    {
        result = m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND );
        CheckFMODResult( result );
    }
    else
    {
        result = m_pSystem->getDriverCaps( 0, &caps, 0, &speakermode );
        CheckFMODResult( result );

        result = m_pSystem->setSpeakerMode( speakermode ); /* Set the user selected speaker mode. */
        CheckFMODResult( result );

        if ( caps & FMOD_CAPS_HARDWARE_EMULATED ) /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
        { /* You might want to warn the user about this. */
            result = m_pSystem->setDSPBufferSize( 1024, 10 );
            CheckFMODResult( result );
        }

        result = m_pSystem->getDriverInfo( 0, name, 256, 0 );
        CheckFMODResult( result );

        if ( strstr( name, "SigmaTel" ) ) /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
        {
            result = m_pSystem->setSoftwareFormat( 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR );
            CheckFMODResult( result );
        }
    }

    result = m_pSystem->init( 100, FMOD_INIT_NORMAL, nullptr ); // Initialize FMOD with 100 virtual voices.
    CheckFMODResult( result );

    result = m_pSystem->set3DSettings( 1.0f, 1.0f, 1.0f );
    CheckFMODResult( result );
}

SoundManager::~SoundManager()
{
    for ( auto& pair : m_Sounds )
    {
        ::FMOD::Sound* pSound = pair.second;
        pSound->release();
    }

    CheckFMODResult( m_pSystem->close() );
    CheckFMODResult( m_pSystem->release() );
}

void SoundManager::Update( float delta )
{
    m_Timer += delta;

    m_pSystem->update();

    if ( m_pActivePlaylist && m_pPlaylistSoundInstance && !m_pPlaylistSoundInstance->IsPlaying() )
    {
        PlayNextTrack();
    }

    // Clear any instances which are no longer being references externally
    for ( SoundInstanceList::iterator it = m_SoundInstances.begin(); it != m_SoundInstances.end(); )
    {
        if ( it->use_count() == 1 && ( *it )->IsPlaying() == false )
        {
            it = m_SoundInstances.erase( it );
        }
        else
        {
            ++it;
        }
    }

	// Force the update of the music's volume to guarantee that it reflects the settings as the player
	// modifies them.
	if ( m_pPlaylistSoundInstance && m_pPlaylistSoundInstance->IsPlaying() )
	{
		m_pPlaylistSoundInstance->SetVolume( 1.0f );
	}
}

// Returns the base Sound associated with a ResourceSound. 
// If one hasn't been created yet, then do so.
::FMOD::Sound* SoundManager::GetSound( ResourceSound* pResourceSound )
{
    SoundMap::const_iterator it = m_Sounds.find( pResourceSound->GetFilename().GetName() );
    if ( it == m_Sounds.cend() )
    {
        return CreateSound( pResourceSound );
    }
    else
    {
        return it->second;
    }
}

// Creates a base Sound. Only one should exist per ResourceSound, so the
// result is cached for repeated accesses.
::FMOD::Sound* SoundManager::CreateSound( ResourceSound* pResourceSound )
{
    unsigned int flags = FMOD_DEFAULT;

    if ( pResourceSound->IsPlaylist() == false )
    {
        if ( !pResourceSound->IsEffect() && !pResourceSound->IsStreamed() )
        {
            FrameWork::GetLogger()->LogWarning( "SoundManager::CreateSound ('%s'): Unable to create sound, resource not set as streaming or effect.", pResourceSound->GetFilename().GetFullPath().c_str() );
            return nullptr;
        }

        if ( pResourceSound->IsHardwareMixed() )
        {
            flags |= FMOD_HARDWARE;
        }
        else if ( pResourceSound->IsSoftwareMixed() )
        {
            flags |= FMOD_SOFTWARE;
        }

        if ( pResourceSound->IsLooping() )
        {
            flags |= FMOD_LOOP_NORMAL;
        }

        if ( pResourceSound->Is3D() )
        {
            flags |= FMOD_3D;
        }
    }

    ::FMOD::Sound* pSound = nullptr;
    FMOD_RESULT result;
    if ( pResourceSound->IsStreamed() )
    {
        result = m_pSystem->createStream( pResourceSound->GetFilename().GetFullPath().c_str(), flags, nullptr, &pSound );
    }
    else
    {
        result = m_pSystem->createSound( pResourceSound->GetFilename().GetFullPath().c_str(), flags, nullptr, &pSound );
    }

    CheckFMODResult( result );

    if ( pSound != nullptr )
    {
        m_Sounds[ pResourceSound->GetFilename().GetName() ] = pSound;
    }

    return pSound;
}

SoundInstanceSharedPtr SoundManager::CreateSoundInstance( ResourceSound* pResourceSound )
{
    ::FMOD::Sound* pSound = GetSound( pResourceSound );
    if ( pSound == nullptr )
    {
        FrameWork::GetLogger()->LogWarning( "SoundManager::CreateSoundInstance ('%s'): Attempted to create sound instance from invalid sound", pResourceSound->GetFilename().GetFullPath().c_str() );
        return nullptr;
    }
    else if ( UpdateInstancingLimit( pResourceSound ) )
    {
        // 3D sounds start paused - they'll be resumed once the 3D attributes are set
        ::FMOD::Channel* pChannel = nullptr;
        FMOD_RESULT result = m_pSystem->playSound( FMOD_CHANNEL_FREE, pSound, pResourceSound->Is3D(), &pChannel );
        CheckFMODResult( result );

        if ( pChannel != nullptr )
        {
            SoundInstanceSharedPtr soundInstance = std::make_shared<::Genesis::Sound::SoundInstance>();
            soundInstance->Initialise( pResourceSound, pChannel );
            m_SoundInstances.push_back( soundInstance );
            return m_SoundInstances.back();
        }
    }

    return nullptr;
}

void SoundManager::SetPlaylist( ResourceSound* pResourceSound, const std::string& startingTrack /* = "" */, bool shuffle /* = false */ )
{
    if ( pResourceSound == m_pActivePlaylist )
    {
        return;
    }

    if ( pResourceSound->IsPlaylist() == false )
    {
        FrameWork::GetLogger()->LogWarning( "SetPlaylist() failed, '%s' is not a valid playlist resource", pResourceSound->GetFilename().GetFullPath().c_str() );
        return;
    }
    else
    {
        if ( m_pActivePlaylist != nullptr && m_pPlaylistSoundInstance != nullptr )
        {
            m_pPlaylistSoundInstance->Stop();
        }

        m_pActivePlaylist = pResourceSound;
        m_pActivePlaylist->Initialise();

        CacheTracks( shuffle );

        if ( m_CachedTracks.empty() == false )
        {
            if ( startingTrack.empty() )
            {
                PlayNextTrack();
            }
            else
            {
                PlayTrack( startingTrack );
            }
        }
    }
}

void SoundManager::CacheTracks( bool shuffle /* = false */ )
{
    m_PlaylistSoundIndex = -1;
    m_CachedTracks.clear();
    if ( m_pActivePlaylist == nullptr )
        return;

    ::FMOD::Sound* pPlaylistSound = GetSound( m_pActivePlaylist );
    int numSongs = 0;
    FMOD_RESULT result = pPlaylistSound->getNumTags( &numSongs, nullptr );
    CheckFMODResult( result );

    for ( int i = 0; i < numSongs; ++i )
    {
        FMOD_TAG tag;
        result = pPlaylistSound->getTag( "FILE", i, &tag );
        CheckFMODResult( result );

        if ( result == FMOD_OK )
        {
            std::string trackName( (char*)tag.data );
            std::string fullPath = m_pActivePlaylist->GetFilename().GetDirectory() + trackName;
            ResourceSound* pTrackResource = (ResourceSound*)FrameWork::GetResourceManager()->GetResource( fullPath );
            if ( pTrackResource != nullptr )
            {
                m_CachedTracks.push_back( pTrackResource );
            }
        }
    }

    if ( shuffle && m_CachedTracks.size() > 1 )
    {
        std::shuffle( m_CachedTracks.begin(), m_CachedTracks.end(), std::default_random_engine( m_Seed ) );
    }
}

bool SoundManager::HasValidPlaylist() const
{
    if ( m_pActivePlaylist == nullptr )
    {
        return false;
    }

    int numTracks = m_CachedTracks.size();
    if ( numTracks == 0 )
    {
        FrameWork::GetLogger()->LogInfo( "Playlist '%s' has no tracks in it", m_pActivePlaylist->GetFilename().GetName().c_str() );
        return false;
    }

    return true;
}

bool SoundManager::PlayNextTrack()
{
    if ( HasValidPlaylist() == false )
        return false;

    const int numTracks = m_CachedTracks.size();
    const int trackToPlay = ( ++m_PlaylistSoundIndex ) % numTracks;
    ResourceSound* pNextTrackResource = m_CachedTracks[ trackToPlay ];

    FrameWork::GetLogger()->LogInfo( "Next track in queue (%d / %d): '%s'", trackToPlay + 1, numTracks, pNextTrackResource->GetFilename().GetName().c_str() );

    pNextTrackResource->Initialise( SOUND_FLAG_STREAM );

	m_pPlaylistSoundInstance = CreateSoundInstance(pNextTrackResource);
	m_PlaylistSoundIndex = trackToPlay;
	return true;
}

bool SoundManager::PlayTrack( const std::string& trackName )
{
    m_PlaylistSoundIndex = -1;

    if ( HasValidPlaylist() == false )
        return false;

    const int numTracks = m_CachedTracks.size();
    for ( int i = 0; i < numTracks; ++i )
    {
        ResourceSound* pTrackResource = m_CachedTracks[ i ];

        if ( trackName == pTrackResource->GetFilename().GetName() )
        {
            pTrackResource->Initialise( SOUND_FLAG_STREAM );
			m_pPlaylistSoundInstance = CreateSoundInstance(pTrackResource);
			m_PlaylistSoundIndex = i;
			return true;
        }
    }

    return false;
}

void SoundManager::SetListener( const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up )
{
    FMOD_VECTOR fmodPosition;
    FMOD_VECTOR fmodVelocity;
    FMOD_VECTOR fmodForward;
    FMOD_VECTOR fmodUp;

    ToFMODVector( position, &fmodPosition );
    ToFMODVector( velocity, &fmodVelocity );
    ToFMODVector( forward, &fmodForward );
    ToFMODVector( up, &fmodUp );

    m_pSystem->set3DListenerAttributes(
        0,
        &fmodPosition,
        &fmodVelocity,
        0,
        0 );
}

glm::vec3 SoundManager::GetListenerPosition() const
{
    FMOD_VECTOR fmodPosition;
    m_pSystem->get3DListenerAttributes( 0, &fmodPosition, nullptr, nullptr, nullptr );
    return glm::vec3( fmodPosition.x, fmodPosition.y, fmodPosition.z );
}

int SoundManager::GetActiveSoundCount() const
{
    int count = 0;
    m_pSystem->getChannelsPlaying( &count );
    return count;
}

bool SoundManager::UpdateInstancingLimit( ResourceSound* pResourceSound )
{
    if ( pResourceSound->GetInstancingLimit() <= 0.0f )
        return true;

    for ( auto& instanceLimit : m_InstanceLimits )
    {
        if ( instanceLimit.m_pResourceSound == pResourceSound )
        {
            if ( instanceLimit.m_NextAllowedInstance > m_Timer )
            {
                return false;
            }
            else
            {
                instanceLimit.m_NextAllowedInstance = m_Timer + pResourceSound->GetInstancingLimit();
                return true;
            }
        }
    }

    InstanceLimit instanceLimit;
    instanceLimit.m_pResourceSound = pResourceSound;
    instanceLimit.m_NextAllowedInstance = m_Timer + pResourceSound->GetInstancingLimit();
    m_InstanceLimits.push_back( instanceLimit );
    return true;
}

} // namespace Genesis::Sound::Private::FMOD

#endif // USE_FMOD
