// Copyright 2014 Pedro Nunes
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

#include "fleet/fleetbehaviour.h"
#include "ship/ship.fwd.h"

namespace Nullscape
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FleetDoctrine
// This represents the ideal fleet composition for a certain faction.
// E.g. a fleet can be capital heavy or have a preference for large numbers of gunships.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FleetDoctrine
{
public:
	FleetDoctrine();
	FleetDoctrine( FleetBehaviourType behaviourType, float gunships, float battlecruisers, float capitals );
	~FleetDoctrine() {};

	float GetRatio( ShipType type ) const;
	FleetBehaviourType GetBehaviourType() const		{ return m_BehaviourType; }

private:
	float m_Ratio[ static_cast<unsigned int>( ShipType::Count ) ];
	FleetBehaviourType m_BehaviourType;
};

inline float FleetDoctrine::GetRatio( ShipType type ) const
{
	return m_Ratio[ static_cast<int>( type ) ];
}

}
