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

#include <list>
#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "taskmanager.h"

namespace Genesis
{

class ResourceSound;

namespace Sound
{

namespace Private
{
class SoundManagerImpl;
}

class SoundInstance;
using SoundInstanceSharedPtr = std::shared_ptr<SoundInstance>;
using SoundInstanceList = std::list<SoundInstanceSharedPtr>;
using ResourceSoundVector = std::vector<ResourceSound*>;

class SoundManager : public Task
{
public:
    SoundManager();
    virtual ~SoundManager();

    TaskStatus Update( float delta );

    SoundInstanceSharedPtr CreateSoundInstance( ResourceSound* pResourceSound );

    void SetPlaylist( ResourceSound* pResourceSound, const std::string& startingSong = "", bool shuffle = false );

    ResourceSound* GetPlaylistResource() const;
    SoundInstanceSharedPtr GetCurrentSong() const;

    const SoundInstanceList& GetSoundInstances() const;

    void SetListener( const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up );
    glm::vec3 GetListenerPosition() const;

    int GetActiveSoundCount() const;

private:
    std::unique_ptr<Private::SoundManagerImpl> m_pImpl;
};

} // namespace Sound
} // namespace Genesis