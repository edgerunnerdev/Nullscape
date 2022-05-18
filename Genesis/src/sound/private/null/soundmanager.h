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

#include "sound/private/soundmanagerimpl.h"

namespace Genesis::Sound::Private::Null
{

class SoundManager : public SoundManagerImpl
{
public:
    SoundManager();
    virtual ~SoundManager() override;

    virtual void Update( float delta ) override;
    virtual SoundInstanceSharedPtr CreateSoundInstance( ResourceSound* pResourceSound ) override;
    virtual void SetPlaylist( ResourceSound* pResourceSound, const std::string& startingSong, bool shuffle ) override;
    virtual ResourceSound* GetPlaylistResource() const override;
    virtual SoundInstanceSharedPtr GetCurrentSong() const override;
    virtual const SoundInstanceList& GetSoundInstances() const override;
    virtual void SetListener( const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up ) override;
    virtual glm::vec3 GetListenerPosition() const override;
    virtual int GetActiveSoundCount() const override;
};

} // namespace Genesis::Sound::Private::Null
