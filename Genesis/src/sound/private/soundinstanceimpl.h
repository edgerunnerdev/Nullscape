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

#include <glm/vec3.hpp>

namespace Genesis
{
    class ResourceSound;
}

namespace Genesis::Sound::Private
{

class SoundInstanceImpl
{
public:
    virtual ~SoundInstanceImpl() {}
    virtual void Initialise( ResourceSound* pResourceSound, void* pData ) = 0;
    virtual bool IsPlaying() const = 0;
    virtual void Stop() = 0;

    virtual unsigned int GetLength() const = 0;
    virtual unsigned int GetPosition() const = 0;
    virtual float GetAudability() const = 0;

    virtual ResourceSound* GetResource() const = 0;

    virtual void SetMinimumDistance( float value ) = 0;
    virtual void Set3DAttributes( const glm::vec3* pPosition = nullptr, const glm::vec3* pVelocity = nullptr ) = 0;
    virtual void Get3DAttributes( glm::vec3* pPosition = nullptr, glm::vec3* pVelocity = nullptr ) = 0;
    virtual void SetVolume( float value ) = 0;
    virtual float GetVolume() const = 0;
};

} // namespace Genesis::Sound::Private
