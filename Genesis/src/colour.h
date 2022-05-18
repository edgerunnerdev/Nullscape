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

#include "glm/vec4.hpp"

namespace Genesis
{

class Colour
{
public:
    Colour()
        : r( 1.0f )
        , g( 1.0f )
        , b( 1.0f )
        , a( 1.0f ){};
    Colour( float R, float G, float B, float A = 1.0f )
        : r( R )
        , g( G )
        , b( B )
        , a( A ){};
    ~Colour(){};

    void Set( float r, float g, float b, float a );
    glm::vec4 glm() const;

    float r;
    float g;
    float b;
    float a;
};

inline void Colour::Set( float red, float green, float blue, float alpha )
{
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

inline glm::vec4 Colour::glm() const
{
    return glm::vec4( r, g, b, a );
}
}
