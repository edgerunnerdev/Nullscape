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

#include <genesis.h>
#include <math/misc.h>

#include "fleet/fleetcommand.h"
#include "fleet/fleetspawner.h"
#include "fleet/fleet.h"
#include "ship/hexgrid.h"
#include "ship/shipinfo.h"
#include "ship/hyperspacecore.h"
#include "sector/sector.h"
#include "blackboard.h"
#include "player.h"
#include "hexterminate.h"

namespace Nullscape
{

bool FleetSpawner::Spawn( FleetSharedPtr pFleet, Sector* pSector, ShipVector* pSpawnedShips, float x, float y )
{
	if ( pFleet->GetFaction() == g_pGame->GetPlayerFaction() )
	{
		return SpawnFleetPlayer( pFleet, pSector, pSpawnedShips, x, y );
	}
	else
	{
		FleetFormation formation = FleetFormation::Cohesive;	// TODO: Implement other formations
		return SpawnFleetAI( pFleet, pSector, pSpawnedShips, x, y, formation );
	}
}

bool FleetSpawner::SpawnFleetAI( FleetSharedPtr pFleet, Sector* pSector, ShipVector* pSpawnedShips, float spawnPointX, float spawnPointY, FleetFormation formation )
{
	CheckFirstEncounter( pFleet->GetFaction() );

	const ShipInfoList& shipsToSpawnList = pFleet->GetShips();
	if ( shipsToSpawnList.empty() )
	{
		return false;
	}

	ShipInfoVector shipsToSpawn( shipsToSpawnList.begin(), shipsToSpawnList.end() );
	if ( pFleet->HasFlagship() )
	{
		CheckFlagshipEncounter( pFleet->GetFaction() );
	}

	FleetCommandUniquePtr pFleetCommand( new FleetCommand );

	ShipSpawnDataVector fleetSpawnData;
	GetSpawnData( shipsToSpawn, spawnPointX, spawnPointY, fleetSpawnData, formation );

	for ( int i = 0, s = static_cast<int>(shipsToSpawn.size()); i < s; ++i )
	{
		Ship* pShip = SpawnShip( pSector, pFleet, shipsToSpawn[ i ], fleetSpawnData[ i ] );

		if ( pFleetCommand->HasLeader() == false )
		{
			pFleetCommand->AssignLeader( pShip );
		}
		else
		{
			pFleetCommand->AssignShip( pShip );
		}

		if ( pSpawnedShips != nullptr )
		{
			pSpawnedShips->push_back( pShip );
		}
	}

	pFleetCommand->SetupRelationships();

	pSector->AddFleetCommand( std::move( pFleetCommand ) );

	return ( shipsToSpawn.size() > 0 );
}

bool FleetSpawner::SpawnFleetPlayer( FleetSharedPtr pFleet, Sector* pSector, ShipVector* pSpawnedShips, float spawnPointX, float spawnPointY )
{
	Ship* pPlayerShip = g_pGame->GetPlayer()->CreateShip( spawnPointX, spawnPointY );
	pSector->AddShip( pPlayerShip );
	return true;
}

void FleetSpawner::GetSpawnData( ShipInfoVector shipsToSpawn, float spawnPointX, float spawnPointY, ShipSpawnDataVector& fleetSpawnData, FleetFormation formation )
{
	float spacing = 200.0f;
	if ( formation == FleetFormation::Cohesive )
	{
		// TODO: Revise cohesive formation to take into account ship types
		int side = static_cast<int>( ceil( sqrt( shipsToSpawn.size() ) ) );
		ShipSpawnData spawnData;
		for ( int i = 0, s = static_cast<int>(shipsToSpawn.size()); i < s; ++i )
		{
			spawnData.m_PositionX = spawnPointX + static_cast<float>( i % side ) * spacing - ( side * spacing ) / 2.0f;
			spawnData.m_PositionY = spawnPointY + floor( static_cast<float>( i ) / side ) * spacing - ( side * spacing ) / 2.0f;
			fleetSpawnData.push_back( spawnData );
		}
	}
	else if ( formation == FleetFormation::Escort )
	{
		const float deltaStagger = 90.0f;

		// The first ship of the vector is assumed to be the centre of the formation
		ShipSpawnData spawnData;
		spawnData.m_PositionX = spawnPointX;
		spawnData.m_PositionY = spawnPointY;
		fleetSpawnData.push_back( spawnData );

		float anchorX = spawnPointX;
		float anchorY = spawnPointY;

		// The remaining ships are placed in delta formation around the point ship
		int spacingMultiplier = 1;
		anchorY -= deltaStagger;
		int numShips = static_cast<int>(shipsToSpawn.size());
		for ( int i = 0; i < numShips - 1; ++i )
		{
			spawnData.m_PositionX = anchorX + spacing * spacingMultiplier;
			spawnData.m_PositionY = anchorY;
			fleetSpawnData.push_back( spawnData );

			spacing *= -1.0f;
			if ( i % 2 == 1 )
			{
				spacingMultiplier++;
				anchorY -= deltaStagger;
			}
		}
	}
	else
	{
		// TODO: Implement remaining formations
		SDL_assert( false );
	}
}

Ship* FleetSpawner::SpawnShip( Sector* pSector, FleetSharedPtr pFleet, const ShipInfo* pShipInfo, const ShipSpawnData& spawnData )
{
	ShipCustomisationData customisationData;
	customisationData.m_CaptainName = "";
	customisationData.m_ShipName = "";
	customisationData.m_pModuleInfoHexGrid = pShipInfo->GetModuleInfoHexGrid();

	Ship* pShip = new Ship();
	pShip->SetInitialisationParameters(
		pFleet->GetFaction(),
		pFleet,
		customisationData,
		spawnData,
		pShipInfo );

	pShip->Initialise();
	pSector->AddShip( pShip );
	return pShip;
}

void FleetSpawner::CheckFirstEncounter( Faction* pFaction )
{

}

void FleetSpawner::CheckFlagshipEncounter( Faction* pFaction )
{

}

}
