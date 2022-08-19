// Copyright 2022 Pedro Nunes
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

#include "system/astronomicalobject/orbit.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <SDL.h>
#include <glm/trigonometric.hpp>
#include <externalheadersend.hpp>
// clang-format on

namespace Nullscape
{

Orbit::Orbit(float radius, float eccentricity)
    : m_Radius(radius)
    , m_Eccentricity(eccentricity)
{
    SDL_assert(radius > 0.0f);
    SDL_assert(eccentricity >= 0.0f);
    SDL_assert(eccentricity < 1.0f); // Only elliptic orbits are supported.
}

glm::vec2 Orbit::At(float theta) const 
{
    const float x = glm::cos(theta) * m_Radius * (1.0f + m_Eccentricity) + m_Radius * m_Eccentricity;
    const float y = glm::sin(theta) * m_Radius * (1.0f - m_Eccentricity); 
    return glm::vec2(x, y);
}

float Orbit::GetEccentricity() const 
{
    return m_Eccentricity;
}

} // namespace Nullscape