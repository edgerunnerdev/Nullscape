#include <xml.h>
#include "menus/intelwindow.h"
#include "ship/inventory.h"
#include "ship/moduleinfo.h"
#include "ship/ship.h"
#include "ship/shipinfo.h"
#include "perks.h"
#include "player.h"
#include "game.hpp"
#include "xmlaux.h"


namespace Nullscape
{

Player::Player():
m_pShip( nullptr ),
m_pInventory( nullptr )
{
	m_pInventory = new Inventory();
}

Player::Player( const ShipCustomisationData& shipCustomisationData):
m_ShipCustomisationData( shipCustomisationData ),
m_pShip( nullptr ),
m_pInventory( nullptr )
{
	m_pInventory = new Inventory();
}

Player::~Player()
{
	delete m_pInventory;
}

Ship* Player::CreateShip( float spawnPointX, float spawnPointY )
{
	SDL_assert( m_pShip == nullptr );

	ShipSpawnData spawnData;
	spawnData.m_PositionX = spawnPointX;
	spawnData.m_PositionY = spawnPointY;

	m_pShip = new Ship();
	m_pShip->SetInitialisationParameters(
		g_pGame->GetPlayerFaction(),
		g_pGame->GetPlayerFleet(),
		m_ShipCustomisationData,
		spawnData,
		nullptr );

	m_pShip->Initialise();

	return m_pShip;
}

bool Player::Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement )
{
	tinyxml2::XMLElement* pElement = xmlDoc.NewElement( "Player" );
	pRootElement->LinkEndChild( pElement );

	Xml::Write( xmlDoc, pElement, "Version", GetVersion() );
	Xml::Write( xmlDoc, pElement, "CaptainName", m_ShipCustomisationData.m_CaptainName );
	Xml::Write( xmlDoc, pElement, "ShipName", m_ShipCustomisationData.m_ShipName );
	Xml::Write( xmlDoc, pElement, "PlayedTime", g_pGame->GetPlayedTime() );

	bool result = true;
	result &= GetInventory()->Write( xmlDoc, pElement );

	return result;
}

bool Player::Read( tinyxml2::XMLElement* pRootElement )
{
	float playedTime = 0.0f;
	for ( tinyxml2::XMLElement* pElement = pRootElement->FirstChildElement(); pElement != nullptr; pElement = pElement->NextSiblingElement() ) 
	{
		Xml::Serialise( pElement, "CaptainName", m_ShipCustomisationData.m_CaptainName );
		Xml::Serialise( pElement, "ShipName", m_ShipCustomisationData.m_ShipName );
		Xml::Serialise( pElement, "PlayedTime", playedTime );

		if ( std::string( pElement->Value() ) == "Inventory" )
		{
			m_pInventory->Read( pElement );
		}
	}

	m_ShipCustomisationData.m_pModuleInfoHexGrid = m_pInventory->GetHexGrid();

	g_pGame->SetPlayedTime( playedTime );

	return true;
}

}