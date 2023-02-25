// Copyright 2014 Pedro Nunes
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

#pragma once

#include <vector>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <externalheadersend.hpp>
// clang-format on

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SectorInfo
// Contains all the information that is needed to instantiate a Sector.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SectorInfo;
typedef std::vector<SectorInfo*> SectorInfoVector;

class SectorInfo
{
public:
    SectorInfo(int x, int y);

    const glm::ivec2& GetCoordinates(int& x, int& y) const;

protected:
    glm::ivec2 m_Coordinates;
};

inline const glm::ivec2& SectorInfo::GetCoordinates(int& x, int& y) const 
{
    return m_Coordinates;
}

} // namespace Hyperscape
