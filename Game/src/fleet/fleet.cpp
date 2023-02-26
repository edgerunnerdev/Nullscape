// Copyright 2023 Pedro Nunes
//
// This file is part of Hyperscape.
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

#include <fleet/fleet.hpp>

namespace Hyperscape
{

void Fleet::AddShip( EntitySharedPtr& pShip )
{
    m_Ships.push_back( pShip );
}

void Fleet::RemoveShip( EntitySharedPtr& pShipToRemove )
{
    m_Ships.erase(std::remove_if( m_Ships.begin(), m_Ships.end(), [&pShipToRemove]( EntitySharedPtr pShip ) { return pShipToRemove == pShip; }));
}

} // namespace Hyperscape
