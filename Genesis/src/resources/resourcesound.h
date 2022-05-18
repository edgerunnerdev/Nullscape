// Copyright 2014 Pedro Nunes
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

#include <string>

#include "../resourcemanager.h"

namespace Genesis
{

static const int SOUND_FLAG_FX = 1 << 0; // Sound effect which is entirely loaded into memory
static const int SOUND_FLAG_STREAM = 1 << 1; // Sound is streamed (use for music)
static const int SOUND_FLAG_SOFTWARE = 1 << 2; // Sound is mixed in software (for DPS effects, etc). If not set, hardware mixing is used instead.
static const int SOUND_FLAG_LOOPING = 1 << 3; // Sound loops
static const int SOUND_FLAG_PLAYLIST = 1 << 4; // Does this resource contain a playlist?
static const int SOUND_FLAG_INITIALISED = 1 << 5; // Has this sound been initialised?
static const int SOUND_FLAG_3D = 1 << 6; // 3D sound? Needs to have 3D attributes set as well

class ResourceSound : public ResourceGeneric
{
public:
    ResourceSound( const Filename& filename );
    virtual ~ResourceSound();
    virtual ResourceType GetType() const override;
    virtual bool Load() override;

    bool Initialise( int flags = 0 );

    bool IsEffect() const;
    bool IsStreamed() const;
    bool IsSoftwareMixed() const;
    bool IsHardwareMixed() const;
    bool IsLooping() const;
    bool IsPlaylist() const;
    bool Is3D() const;

    void SetInstancingLimit( float time );
    float GetInstancingLimit() const;

    static ResourceSound* LoadAs3D( const std::string& filename );

private:
    unsigned int m_Flags;
    float m_InstancingLimit;
};

inline bool ResourceSound::IsEffect() const
{
    return ( m_Flags & SOUND_FLAG_FX ) != 0;
}

inline bool ResourceSound::IsStreamed() const
{
    return ( m_Flags & SOUND_FLAG_STREAM ) != 0;
}

inline bool ResourceSound::IsSoftwareMixed() const
{
    return ( m_Flags & SOUND_FLAG_SOFTWARE ) != 0;
}

inline bool ResourceSound::IsHardwareMixed() const
{
    return !IsSoftwareMixed();
}

inline bool ResourceSound::IsLooping() const
{
    return ( m_Flags & SOUND_FLAG_LOOPING ) != 0;
}

inline bool ResourceSound::IsPlaylist() const
{
    return ( m_Flags & SOUND_FLAG_PLAYLIST ) != 0;
}

inline bool ResourceSound::Is3D() const
{
    return ( m_Flags & SOUND_FLAG_3D ) != 0;
}

inline void ResourceSound::SetInstancingLimit( float value )
{
    m_InstancingLimit = value;
}

inline float ResourceSound::GetInstancingLimit() const
{
    return m_InstancingLimit;
}

inline ResourceType ResourceSound::GetType() const
{
    return ResourceType::Sound;
}
}