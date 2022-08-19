#pragma once

#include <list>

#include <coredefines.h>

#include "ship/ship.h"
#include "serialisable.h"

namespace Nullscape
{

class Inventory;
class Perks;

class Player: public Serialisable
{
public:
	Player();
	Player( const ShipCustomisationData& shipCustomisationData);
	~Player();

	Ship*					CreateShip( float spawnPointX, float spawnPointY );

	ShipCustomisationData&	GetShipCustomisationData()	{ return m_ShipCustomisationData; }
	Ship*					GetShip() const				{ return m_pShip; }
	Inventory*				GetInventory() const		{ return m_pInventory; }

	// Serialisable
	virtual bool			Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement ) override;
	virtual bool			Read( tinyxml2::XMLElement* pRootElement ) override;
	virtual int				GetVersion() const override { return 2; }
	virtual void			UpgradeFromVersion( int version ) override {}

private:
	ShipCustomisationData	m_ShipCustomisationData;
	Ship*					m_pShip;
	Inventory*				m_pInventory;
};

GENESIS_DECLARE_SMART_PTR(Player);

}
