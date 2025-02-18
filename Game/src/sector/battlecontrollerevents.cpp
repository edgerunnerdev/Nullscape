// Copyright 2023 Pedro Nunes
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

#include "sector/battlecontrollerevents.hpp"

namespace Hyperscape
{

BattleEvent::BattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry )
    : m_pFrom( pFromEntity )
    , m_pTo( pToEntity )
{
}

FireProjectileBattleEvent::FireProjectileBattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry )
    : BattleEvent( pFromEntity, pToEntity, expiry )
{
}

void FireProjectileBattleEvent::OnExpiry()
{
}

IncomingProjectileBattleEvent::IncomingProjectileBattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry )
    : BattleEvent( pFromEntity, pToEntity, expiry )
{
}

void IncomingProjectileBattleEvent::OnExpiry()
{
}

} // namespace Hyperscape
