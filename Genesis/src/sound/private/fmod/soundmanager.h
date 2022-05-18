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

#pragma once

#if USE_FMOD

#include <list>
#include <memory>
#include <unordered_map>

#include "sound/private/soundmanagerimpl.h"

namespace FMOD
{
class Channel;
class Sound;
class System;
}

namespace Genesis::Sound::Private::FMOD
{

class SoundManager : public SoundManagerImpl
{
public:
    SoundManager();
    virtual ~SoundManager();

    virtual void Update( float delta ) override;
    virtual SoundInstanceSharedPtr CreateSoundInstance( ResourceSound* pResourceSound ) override;
    virtual void SetPlaylist( ResourceSound* pResourceSound, const std::string& startingSong, bool shuffle ) override;
    virtual ResourceSound* GetPlaylistResource() const override;
    virtual SoundInstanceSharedPtr GetCurrentSong() const override;
    virtual const SoundInstanceList& GetSoundInstances() const override;
    virtual void SetListener( const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up ) override;
    virtual glm::vec3 GetListenerPosition() const override;
    virtual int GetActiveSoundCount() const override;

private:
    ::FMOD::Sound* CreateSound( ResourceSound* pResourceSound );
    ::FMOD::Sound* GetSound( ResourceSound* pResourceSound );
    void CacheTracks( bool shuffle = false );
    bool PlayNextTrack();
    bool PlayTrack( const std::string& songName );
    bool HasValidPlaylist() const;
    bool UpdateInstancingLimit( ResourceSound* pResourceSound );

    ::FMOD::System* m_pSystem;
    SoundInstanceList m_SoundInstances;
    ResourceSound* m_pActivePlaylist;
    SoundInstanceSharedPtr m_pPlaylistSoundInstance;
    int m_PlaylistSoundIndex;
    ResourceSoundVector m_CachedTracks;

    struct InstanceLimit
    {
        ResourceSound* m_pResourceSound;
        float m_NextAllowedInstance;
    };
    typedef std::vector<InstanceLimit> InstanceLimitVector;
    InstanceLimitVector m_InstanceLimits;
    float m_Timer;
    unsigned int m_Seed;

    using SoundMap = std::unordered_map<std::string, ::FMOD::Sound*>;
    SoundMap m_Sounds;
};

inline ResourceSound* SoundManager::GetPlaylistResource() const
{
    return m_pActivePlaylist;
}

inline SoundInstanceSharedPtr SoundManager::GetCurrentSong() const
{
    return m_pPlaylistSoundInstance;
}

inline const SoundInstanceList& SoundManager::GetSoundInstances() const
{
    return m_SoundInstances;
}

} // namespace Genesis::Sound::Private::FMOD

#endif // USE_FMOD
