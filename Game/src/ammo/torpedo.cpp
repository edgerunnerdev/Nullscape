// Copyright 2015 Pedro Nunes
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

#include "ammo/torpedo.h"
#include "trail/trail.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Torpedo
///////////////////////////////////////////////////////////////////////////////

Torpedo::Torpedo()
{
    SetGlowColour(Genesis::Colour(1.0f, 1.0f, 1.0f, 1.0f));
}

MissileType Torpedo::GetType() const
{
    return MissileType::Torpedo;
}

const std::string Torpedo::GetResourceName() const
{
    static const std::string sResourceName("data/models/ammo/torpedo.tmf");
    return sResourceName;
}

void Torpedo::GetTrailProperties(float& initialWidth, float& decay, glm::vec4& color) const
{
    initialWidth = 5.0f;
    decay = 2.0f;
    color = glm::vec4(0.6f, 0.2f, 0.2f, 0.5f);
}

}
