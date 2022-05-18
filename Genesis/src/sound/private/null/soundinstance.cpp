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

#include "sound/private/null/soundinstance.h"

namespace Genesis::Sound::Private::Null
{

SoundInstance::SoundInstance()
{

}

SoundInstance::~SoundInstance()
{
    
}

void SoundInstance::Initialise( ResourceSound* pResourceSound, void* pData )
{

}

bool SoundInstance::IsPlaying() const
{
    return false;
}

void SoundInstance::Stop()
{

}

unsigned int SoundInstance::GetLength() const
{
    return 0u;
}

unsigned int SoundInstance::GetPosition() const
{
    return 0u;
}

float SoundInstance::GetAudability() const
{
    return 0.0f;
}

ResourceSound* SoundInstance::GetResource() const
{
    return nullptr;
}

void SoundInstance::SetMinimumDistance( float value )
{

}
    
void SoundInstance::Set3DAttributes( const glm::vec3* pPosition, const glm::vec3* pVelocity )
{

}

void SoundInstance::Get3DAttributes( glm::vec3* pPosition, glm::vec3* pVelocity )
{
    if ( pPosition != nullptr )
    {
        *pPosition = glm::vec3( 0.0f );
    }

    if ( pVelocity != nullptr )
    {
        *pVelocity = glm::vec3( 0.0f );
    } 
}

void SoundInstance::SetVolume( float value )
{

}

float SoundInstance::GetVolume() const
{
    return 0.0f;
}

} // namespace Genesis::Sound::Private
