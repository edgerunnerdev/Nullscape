#include <cassert>
#include <algorithm>
#include <math/misc.h>
#include <xml.h>

#include <genesis.h>
#include <log.hpp>

#include "fleet/fleet.h"
#include "fleet/fleetbehaviour.h"
#include "fleet/fleetdoctrine.h"
#include "fleet/fleetspawner.h"
#include "ship/shipinfo.h"
#include "ship/ship.h"
#include "sector/sector.h"
#include "player.h"
#include "hexterminate.h"
#include "globals.h"
#include "xmlaux.h"

namespace Hyperscape
{
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fleet
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Fleet::Fleet() :
m_pFaction( nullptr ),
m_pInitialSector( nullptr ),
m_pDestinationSector( nullptr ),
m_State( FleetState::Idle ),
m_pBehaviour( nullptr ),
m_Points( 0 ),
m_AutoResolvePoints( 0 ),
m_Terminating( false ),
m_RegionalFleet( false ),
m_HasFlagship( false ),
m_ImmunityTimer( 0.0f )
{

}

Fleet::~Fleet() 
{
	delete m_pBehaviour;
}

void Fleet::Initialise( Faction* pFaction, const SectorInfo* pInitialSector )
{
	m_pFaction = pFaction;
	m_pInitialSector = pInitialSector;

	int x, y;
	m_pInitialSector->GetCoordinates( x, y );
	m_Position = glm::vec2( 
		( static_cast<float>( x ) + 0.5f ) / NumSectorsX, 
		( static_cast<float>( y ) + 0.5f ) / NumSectorsY
	);
	m_Destination = m_Position;

	if ( pFaction->GetFactionId() == FactionId::Player )
	{
		m_HasFlagship = true;
	}

	SetBehaviourFromFaction();
}

void Fleet::SetBehaviourFromFaction()
{
	FleetBehaviourType behaviourType = m_pFaction->GetFleetDoctrine().GetBehaviourType();
	if ( behaviourType == FleetBehaviourType::Roaming )
		m_pBehaviour = new FleetBehaviourRoaming( this, m_pInitialSector );
	else if ( behaviourType == FleetBehaviourType::Raiding )
		m_pBehaviour = new FleetBehaviourRaiding( this );
	else if ( behaviourType == FleetBehaviourType::Expansionist )
		m_pBehaviour = new FleetBehaviourExpansionist( this );
}

void Fleet::Update( float delta )
{
	if ( GetState() == FleetState::Moving )
	{
		float maxMovement = delta / NumSectorsX * FleetSpeed; // Movement in sectors per second
		glm::vec2 diff = m_Destination - m_Position;
		float length = glm::length( diff );
		if ( length > maxMovement )
		{
			glm::vec2 dir = diff / length;
			m_Position += dir * maxMovement;
		}
		else
		{
			m_Position = m_Destination;
			SetState( FleetState::Arrived );

			// If we have arrived, forcibly update our behaviour so we don't hang around without doing anything
			if ( m_pBehaviour != nullptr )
			{
				m_pBehaviour->ProcessTurn();
			}
		}
	}

	m_ImmunityTimer = std::max( 0.0f, m_ImmunityTimer - delta );
}

void Fleet::ProcessTurn()
{
	if ( m_pBehaviour != nullptr )
	{
		m_pBehaviour->ProcessTurn();
	}
}

SectorInfo* Fleet::GetCurrentSector() const
{
	return nullptr;
}

void Fleet::NotifyBattleWon()
{
	if ( m_pBehaviour != nullptr )
	{
		m_pBehaviour->NotifyBattleWon();
	}
}

void Fleet::SetEngaged( bool state )
{ 

}

int Fleet::GetPoints() const
{
	FleetSharedPtr pPlayerFleet = g_pGame->GetPlayerFleet().lock();
	if ( pPlayerFleet != nullptr && pPlayerFleet.get() == this )
	{
		Player* pPlayer = g_pGame->GetPlayer();
		int points = pPlayer->GetShipCustomisationData().m_pModuleInfoHexGrid->GetUsedSlots() * PointsPerModule;

		for ( auto& pShipInfo : m_Ships )
		{
			points += pShipInfo->GetPoints();
		}

		return points;
	}
	else
	{
		return m_Points;
	}
}

bool Fleet::Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement )
{
	using namespace tinyxml2;
	XMLElement* pFleetElement = xmlDoc.NewElement( "Fleet" );
	pRootElement->LinkEndChild( pFleetElement );

	Xml::Write( xmlDoc, pFleetElement, "Version", GetVersion() );

	Xml::Write( xmlDoc, pFleetElement, "PosX", m_Position.x );
	Xml::Write( xmlDoc, pFleetElement, "PosY", m_Position.y );
	Xml::Write( xmlDoc, pFleetElement, "DstX", m_Destination.x );
	Xml::Write( xmlDoc, pFleetElement, "DstY", m_Destination.y );
	Xml::Write( xmlDoc, pFleetElement, "Flagship", m_HasFlagship );
	Xml::Write( xmlDoc, pFleetElement, "State", m_State );

	if ( m_pInitialSector != nullptr )
	{
		int x, y;
		m_pInitialSector->GetCoordinates( x, y );
		Xml::Write( xmlDoc, pFleetElement, "InitialSectorX", x );
		Xml::Write( xmlDoc, pFleetElement, "InitialSectorY", y );
	}

	for ( auto& pShipInfo : m_Ships )
	{
		Xml::Write( xmlDoc, pFleetElement, "Ship", pShipInfo->GetName() );
	}

	return true;
}

bool Fleet::Read( tinyxml2::XMLElement* pRootElement )
{
	float posX = 0.0f;
	float posY = 0.0f;
	float dstX = 0.0f;
	float dstY = 0.0f;

	int initialSectorX = -1;
	int initialSectorY = -1;

	bool hasFlagship = false;
	int version = 1;

	ShipInfoManager* pShipInfoManager = g_pGame->GetShipInfoManager();

	for ( tinyxml2::XMLElement* pChildElement = pRootElement->FirstChildElement(); pChildElement != nullptr; pChildElement = pChildElement->NextSiblingElement() ) 
	{
		Xml::Serialise( pChildElement, "Version", version );

		Xml::Serialise( pChildElement, "PosX", posX );
		Xml::Serialise( pChildElement, "PosY", posY );
		Xml::Serialise( pChildElement, "DstX", dstX );
		Xml::Serialise( pChildElement, "PosY", dstY );

		Xml::Serialise( pChildElement, "Points", m_Points ); // Keep in place so version 2 can be upgraded to version 3
		Xml::Serialise( pChildElement, "Flagship", hasFlagship );

		Xml::Serialise( pChildElement, "InitialSectorX", initialSectorX );
		Xml::Serialise( pChildElement, "InitialSectorY", initialSectorY );

		Xml::Serialise( pChildElement, "State", m_State );

		std::string shipName;
		if ( Xml::Serialise( pChildElement, "Ship", shipName ) )
		{
			// When we read the player's fleet, we want to spawn the Empire's equivalent of those ships.
			// The naming of the ships should actually be changed to avoid this but it would break backwards compability of the save games.
			Faction* pFaction = GetFaction();
			if ( pFaction->GetFactionId() == FactionId::Player )
			{
				pFaction = g_pGame->GetFaction( FactionId::Empire );
			}

			const ShipInfo* pShipInfo = pShipInfoManager->Get( pFaction, shipName );
			if ( pShipInfo != nullptr )
			{
				AddShip( pShipInfo );
			}
		}
	}

	SDL_assert_release( initialSectorX >= 0 && initialSectorX < NumSectorsX );
	SDL_assert_release( initialSectorY >= 0 && initialSectorY < NumSectorsY );

	if ( version != GetVersion() )
	{
		UpgradeFromVersion( version );
	}

	return true;
}

void Fleet::UpgradeFromVersion( int version )
{
    Genesis::Core::Log::Info() << "Fleet::UpgradeFromVersion(): " << version << " -> " << GetVersion();
}

void Fleet::SetDestination( float x, float y )		
{ 
	m_Destination = glm::vec2( x, y );
	m_State = FleetState::Moving; 
}

void Fleet::SetDestinationSector( const SectorInfo* pSectorInfo )
{
	int coordX, coordY;
	pSectorInfo->GetCoordinates( coordX, coordY );
	SetDestination( ((float)coordX + 0.5f) / NumSectorsX, ((float)coordY + 0.5f) / NumSectorsY );
}

bool Fleet::IsInRangeOf( FleetWeakPtr pOtherFleetWeakPtr ) const
{
	return false;
}

void Fleet::GenerateProceduralFleet( int pointsToSpend )
{
	m_Ships.clear();
	m_Points = 0;

	const ShipInfoVector& factionShips = g_pGame->GetShipInfoManager()->Get( GetFaction() );
	ShipInfoVector factionShipsByType[ static_cast<int>( ShipType::Count ) ];

	for ( auto& pFactionShip : factionShips )
	{
		if ( pFactionShip->IsSpecial() == false )
		{
			factionShipsByType[ static_cast<int>( pFactionShip->GetShipType() ) ].push_back( pFactionShip );
		}
	}

	// Validate available ships against fleet doctrine - we don't want to try to spawn e.g. gunships if this faction doesn't
	// actually have any gunship templates available.
	const FleetDoctrine& doctrine = GetFaction()->GetFleetDoctrine();
	for ( int i = 0; i < static_cast<int>( ShipType::Count ); ++i )
	{
		if ( doctrine.GetRatio( static_cast<ShipType>( i ) ) > 0.0f && factionShipsByType[ i ].empty() )
		{
            Genesis::Core::Log::Error() << "Faction " << GetFaction()->GetName() << "doesn't have the ship types required by its doctrine.";
			return;
		}
	}

	// Select which ships to spawn, based on the fleet's doctrine
	const int minimumPoints = FindCheapestShipCost();
	pointsToSpend = std::max( minimumPoints, pointsToSpend ); // Make sure that we always spawn at least one ship
	while ( pointsToSpend >= minimumPoints )
	{
		const int type = rand() % static_cast<int>( ShipType::Count );
		const float f = gRand();
		if ( f < doctrine.GetRatio( static_cast< ShipType> ( type ) ) )
		{
			const ShipInfo* pInfo = factionShipsByType[ type ].at( rand() % factionShipsByType[ type ].size() );
			if ( pInfo->GetPoints() <= pointsToSpend )
			{
				AddShip( pInfo );
				pointsToSpend -= pInfo->GetPoints();
			}
		}
	}
}

void Fleet::GenerateFlagshipFleet()
{
    const ShipInfoVector& flagshipFleetShips = GetFaction()->GetFlagshipFleetShips();
    SDL_assert( flagshipFleetShips.empty() == false );

    for ( auto& shipInfo : flagshipFleetShips )
    {
        AddShip( shipInfo );
    }
}

int Fleet::FindCheapestShipCost() const
{
	int points = INT_MAX;

	const ShipInfoVector& factionShips = g_pGame->GetShipInfoManager()->Get( GetFaction() );
	const FleetDoctrine& doctrine = GetFaction()->GetFleetDoctrine();

	for ( auto& pShipInfo : factionShips )
	{
		if ( pShipInfo->IsSpecial() )
		{
			continue;
		}

		if ( doctrine.GetRatio( pShipInfo->GetShipType() ) > 0.0f && pShipInfo->GetPoints() < points )
		{
			points = pShipInfo->GetPoints();
		}
	}

	return points;
}

void Fleet::AddShip( const std::string& shipName )
{
	ShipInfoManager* pShipInfoManager = g_pGame->GetShipInfoManager();
	const ShipInfo* pShipInfo = pShipInfoManager->Get( GetFaction(), shipName );
	if ( pShipInfo != nullptr )
	{
		AddShip( pShipInfo );
	}
}

void Fleet::AddShip( const ShipInfo* pShipInfo )
{
	if ( pShipInfo->GetName() == "special_flagship" )
	{
		SDL_assert( m_HasFlagship == false );
		m_HasFlagship = true;
	}
	// The player's ship is a special case, as it is a flagship but doesn't have the "special_flagship" name.
	else if ( GetFaction()->GetFactionId() == FactionId::Player )
	{
		m_HasFlagship = true;
	}

	m_Ships.push_back( pShipInfo );
	m_Points += pShipInfo->GetPoints();
}

bool Fleet::RemoveShip( const ShipInfo* pShipInfo )
{
	for ( ShipInfoList::iterator it = m_Ships.begin(), itEnd = m_Ships.end(); it != itEnd; ++it )
	{
		if ( *it == pShipInfo )
		{
			m_Ships.erase( it );
			m_Points -= pShipInfo->GetPoints();

			if ( pShipInfo->GetName() == "special_flagship" )
			{
				m_HasFlagship = false;
			}

			return true;
		}
	}

	return false;
}

}