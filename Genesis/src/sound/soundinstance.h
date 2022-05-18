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

#pragma once

#include <list>
#include <memory>

#include "taskmanager.h"

namespace Genesis
{
    class ResourceSound;
}

namespace Genesis::Sound
{

namespace Private
{
    class SoundInstanceImpl;
    using SoundInstanceImplUniquePtr = std::unique_ptr<SoundInstanceImpl>;
}

class SoundInstance
{
public:
    SoundInstance();
    ~SoundInstance();

    void Initialise( ResourceSound* pResourceSound, void* pData );
    bool IsPlaying() const;
    void Stop();

    unsigned int GetLength() const; // Length in milliseconds
    unsigned int GetPosition() const; // Current position in milliseconds
    float GetAudability() const;

    ResourceSound* GetResource() const;

    void SetMinimumDistance( float value );
    void Set3DAttributes( const glm::vec3* pPosition = nullptr, const glm::vec3* pVelocity = nullptr );
    void Get3DAttributes( glm::vec3* pPosition = nullptr, glm::vec3* pVelocity = nullptr );
    void SetVolume( float value );
    float GetVolume() const;

private:
    Private::SoundInstanceImplUniquePtr m_pImpl;
};

} // namespace Genesis
