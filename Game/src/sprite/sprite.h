// Copyright 2016 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <colour.h>

namespace Nullscape
{

///////////////////////////////////////////////////////////////////////////////
// Sprite
// It needs to be passed to the SpriteManager every frame it needs to be
// rendered and deals entirely in world space coordinates.
///////////////////////////////////////////////////////////////////////////////

class Sprite
{
public:
    Sprite()
        : m_Position(0.0f, 0.0f, 0.0f)
        , m_Colour(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Size(1.0f)
        , m_SpriteIdx(0)
    {
    }

    Sprite(const glm::vec3& position, const Genesis::Colour& colour, float size, unsigned int spriteIdx)
        : m_Position(position)
        , m_Colour(colour)
        , m_Size(size)
        , m_SpriteIdx(spriteIdx)
    {
    }

    const glm::vec3& GetPosition() const;
    const Genesis::Colour& GetColour() const;
    float GetSize() const;
    unsigned int GetIndex() const;

private:
    glm::vec3 m_Position;
    Genesis::Colour m_Colour;
    float m_Size;
    unsigned int m_SpriteIdx;
};

inline const glm::vec3& Sprite::GetPosition() const
{
    return m_Position;
}

inline const Genesis::Colour& Sprite::GetColour() const
{
    return m_Colour;
}

inline float Sprite::GetSize() const
{
    return m_Size;
}

inline unsigned int Sprite::GetIndex() const
{
    return m_SpriteIdx;
}

} // namespace Nullscape
