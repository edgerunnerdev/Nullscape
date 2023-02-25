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

#include <algorithm>
#include <math/misc.h>
#include <genesis.h>
#include <xml.h>
#include "faction/faction.h"
#include "misc/randomshuffle.h"
#include "ship/shipinfo.h"
#include "achievements.h"
#include "blackboard.h"
#include "game.hpp"
#include "globals.h"
#include "stringaux.h"
#include "xmlaux.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////
// Faction
///////////////////////////////////////////////////////////

Faction::Faction( const FactionInfo& info, FactionId id ):
m_Info( info ),
m_FactionId( id ),
m_Turn( 0 ),
m_NextTurnTimer( 0.0f ),
m_pHomeworld( nullptr ),
m_StartedWithHomeworld( false ),
m_InitialPresence( FactionPresence::Standard )
{

}

void Faction::Update( float delta )
{

}

void Faction::PostUpdate()
{

}

void Faction::ProcessTurn()
{

}

void Faction::ForceNextTurn()
{

}

void Faction::BuildFleets( const SectorInfoVector& shipyards )
{

}

void Faction::ProcessTurnFleets()
{

}

// If a Faction is collapsing then for every turn they have a chance of losing control of sectors. 
void Faction::ProcessCollapse()
{

}

void Faction::AddControlledSector( SectorInfo* pSector, bool immediate, bool takenByPlayer )
{

}

void Faction::RemoveControlledSector( SectorInfo* pSectorToRemove, bool immediate /* = false */ )
{

}

bool Faction::IsFlagshipSpawned() const
{
	return false;
}

bool Faction::IsFlagshipDestroyed() const
{
	return false;
}

bool Faction::sIsEnemyOf( Faction* pFactionA, Faction* pFactionB )
{
	return true;
}

const LootProbability& Faction::GetLootProbability( bool isFlagship ) const
{ 
	return m_Info.m_LootProbability;
}

int	Faction::GetConquestReward( const SectorInfo* pSectorInfo, bool* pBonusApplied /* = nullptr */ ) const 
{
	return 0;
}

bool Faction::IsCollapsing() const
{
	return false;
}

bool Faction::Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement )
{
	return true;
}

bool Faction::Read( tinyxml2::XMLElement* pRootElement )
{
	return true;
}

void Faction::SetInitialPresence( FactionPresence presence )
{
	m_InitialPresence = presence;
}

FactionPresence Faction::GetInitialPresence() const
{
	return m_InitialPresence;
}

const ShipInfoVector& Faction::GetFlagshipFleetShips() const
{ 
	return m_FlagshipFleetShips;
}

void Faction::SetHomeworld( SectorInfo* pSectorInfo )	
{ 
	m_pHomeworld = pSectorInfo; 
}

}