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

#include "sound/soundmanager.h"

namespace Genesis
{

class ResourceSound;

namespace Sound::Private
{

class SoundManagerImpl
{
public:
    virtual ~SoundManagerImpl() {}
    virtual void Update( float delta ) = 0;
    //FMOD::Sound* CreateSound( ResourceSound* pResourceSound );
    virtual SoundInstanceSharedPtr CreateSoundInstance( ResourceSound* pResourceSound ) = 0;
    virtual void SetPlaylist( ResourceSound* pResourceSound, const std::string& startingSong, bool shuffle ) = 0;
    virtual ResourceSound* GetPlaylistResource() const = 0;
    virtual SoundInstanceSharedPtr GetCurrentSong() const = 0;
    virtual const SoundInstanceList& GetSoundInstances() const = 0;
    virtual void SetListener( const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up ) = 0;
    virtual glm::vec3 GetListenerPosition() const = 0;
    virtual int GetActiveSoundCount() const = 0;
};

} // namespace Sound::Private
} // namespace Genesis
