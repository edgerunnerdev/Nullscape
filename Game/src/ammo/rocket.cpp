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

#include "ammo/rocket.h"

#include "trail/trail.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Rocket
///////////////////////////////////////////////////////////////////////////////

Rocket::Rocket()
{
    SetGlowSize(25.0f);
    SetGlowColour(Genesis::Colour(1.0f, 0.4f, 0.0f, 1.0f));
}

MissileType Rocket::GetType() const
{
    return MissileType::Rocket;
}

const std::string Rocket::GetResourceName() const
{
    static const std::string sResourceName("data/models/ammo/missile.tmf");
    return sResourceName;
}

void Rocket::GetTrailProperties(float& initialWidth, float& decay, glm::vec4& color) const
{
    initialWidth = 1.5f;
    decay = 0.2f;
    color = glm::vec4(0.6f, 0.2f, 0.0f, 1.0f);
}

} // namespace Hyperscape
