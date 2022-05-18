// Copyright 2014 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#include <cassert>

#include <SDL.h>

#include "fleet/fleetdoctrine.h"

namespace Hexterminate
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FleetDoctrine
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FleetDoctrine::FleetDoctrine()
{
	m_BehaviourType = FleetBehaviourType::None;
	m_Ratio[ (int)ShipType::Gunship ]		= 1.0f;
	m_Ratio[ (int)ShipType::Battlecruiser ]	= 0.0f;
	m_Ratio[ (int)ShipType::Capital ]		= 0.0f;
}

FleetDoctrine::FleetDoctrine( FleetBehaviourType behaviourType, float gunships, float battlecruisers, float capitals )
{
	m_BehaviourType = behaviourType;

	float l = ( gunships + battlecruisers + capitals );
	SDL_assert( l > 0.0f );
	m_Ratio[ (int)ShipType::Gunship ] = gunships / l;
	m_Ratio[ (int)ShipType::Battlecruiser ] = battlecruisers / l;
	m_Ratio[ (int)ShipType::Capital ] = capitals / l;
}

}