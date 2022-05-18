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

#if USE_FMOD

#include <fmod.h>
#include <fmod.hpp>

#include "sound/private/soundinstanceimpl.h"

namespace Genesis
{
    class ResourceSound;
}

namespace Genesis::Sound::Private::FMOD
{

class SoundInstance : public SoundInstanceImpl 
{
public:
    SoundInstance();
    virtual ~SoundInstance() override;
    virtual void Initialise( ResourceSound* pResourceSound, void* pData ) override;
    virtual bool IsPlaying() const override;
    virtual void Stop() override;

    virtual unsigned int GetLength() const override;
    virtual unsigned int GetPosition() const override;
    virtual float GetAudability() const override;

    virtual ResourceSound* GetResource() const override;

    virtual void SetMinimumDistance( float value ) override;
    virtual void Set3DAttributes( const glm::vec3* pPosition, const glm::vec3* pVelocity ) override;
    virtual void Get3DAttributes( glm::vec3* pPosition, glm::vec3* pVelocity ) override;
    virtual void SetVolume( float value ) override;
    virtual float GetVolume() const override;

private:
    ResourceSound* m_pResource;
    ::FMOD::Channel* m_pChannel;
    bool m_PlaybackPending;
};

} // namespace Genesis::Sound::Private::FMOD

#endif // USE_FMOD
