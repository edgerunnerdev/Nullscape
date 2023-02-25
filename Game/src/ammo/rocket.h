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

#pragma once

#include "ammo/missile.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Rocket
///////////////////////////////////////////////////////////////////////////////

class Rocket : public Missile
{
public:
    Rocket();

protected:
    virtual MissileType GetType() const override;
    virtual const std::string GetResourceName() const override;
    virtual void GetTrailProperties(float& initialWidth, float& decay, glm::vec4& color) const override;
};

} // namespace Hyperscape
