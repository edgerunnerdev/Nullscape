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
#include <glm/vec3.hpp>

#include "genesis.h"
#include "logger.h"

namespace Genesis::Sound::Private::FMOD
{

inline void CheckFMODResult( FMOD_RESULT result )
{
    if ( result != FMOD_OK )
    {
        Genesis::FrameWork::GetLogger()->LogWarning( "FMOD error %d: %s", result, FMOD_ErrorString( result ) );
    }
}

inline void ToFMODVector(const glm::vec3& inVector, FMOD_VECTOR* pOutVector)
{
	pOutVector->x = inVector.x;
	pOutVector->y = inVector.y;
	pOutVector->z = inVector.z;
}

} // namespace Genesis::Sound::Private::FMOD

#endif // USE_FMOD
