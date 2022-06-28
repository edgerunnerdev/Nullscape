// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#include "system/astronomicalobject/orbit.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <SDL.h>
#include <externalheadersend.hpp>
// clang-format on

namespace Hyperscape
{

Orbit::Orbit(float radius, float eccentricity)
    : m_Radius(radius)
    , m_Eccentricity(eccentricity)
{
    SDL_assert(radius > 0.0f);
    SDL_assert(eccentricity > 0.0f);
    SDL_assert(eccentricity < 1.0f); // Only elliptic orbits are supported.
}

glm::vec2 Orbit::At(float theta) const 
{
    return glm::vec2(0.0f);
}

} // namespace Hyperscape