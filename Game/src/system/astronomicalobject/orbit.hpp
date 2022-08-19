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

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <coredefines.h>

namespace Nullscape
{

class Orbit
{
public:
    Orbit(float radius, float eccentricity);
    glm::vec2 At(float theta) const;
    float GetEccentricity() const;

private:
    float m_Radius;
    float m_Eccentricity;
};
GENESIS_DECLARE_SMART_PTR(Orbit);

} // namespace Nullscape