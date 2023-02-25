// Copyright 2014 Pedro Nunes
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

#include <xml.h>
#include <math/misc.h>
#include <genesis.h>
#include <log.hpp>
#include "ship/hexgrid.h"
#include "ship/inventory.h"
#include "gameevents.h"
#include "game.hpp"
#include "xmlaux.h"

namespace Hyperscape
{

void Inventory::AddModule( const std::string& moduleName, unsigned int quantity /* = 1 */, unsigned int cachedQuantity /* = 0 */, bool triggersNotification /* = false */ )
{
	ModuleInfo* pModuleInfo = g_pGame->GetModuleInfoManager()->GetModuleByName( moduleName );
	if ( pModuleInfo == nullptr )
	{
		Genesis::Log::Warning() << "Unknown module: " << moduleName;
	}
	else
	{
		InventoryItemMap::iterator it = m_Items.find( moduleName );
		if ( it == m_Items.end() )
		{
			m_Items[ moduleName ] = ItemData( quantity, cachedQuantity );
		}
		else
		{
			it->second = ItemData(
				gMin( it->second.quantity + quantity, 99 ),
				gMin( it->second.cached + cachedQuantity, 99 ) 
			);
		}

		GameEventManager::Broadcast( new GameEventModuleAcquired( pModuleInfo, quantity, triggersNotification ) ); 
	}
}

void Inventory::RemoveModule( const std::string& moduleName )
{
	InventoryItemMap::iterator it = m_Items.find( moduleName );
	if ( it != m_Items.end() && it->second.quantity > 0 )
	{
		ItemData& itemData = it->second;
		itemData.quantity--;
		if ( itemData.cached > 0 )
		{
			itemData.cached--;
		}
	}
}

int Inventory::GetModuleCount( const std::string& moduleName )
{
	InventoryItemMap::iterator it = m_Items.find( moduleName );
	if ( it == m_Items.end() )
	{
		return 0;
	}
	else
	{
		return it->second.quantity;
	}
}

bool Inventory::Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement )
{
	using namespace tinyxml2;

	XMLElement* pInventoryElement = xmlDoc.NewElement( "Inventory" );
	pRootElement->LinkEndChild( pInventoryElement );

	Xml::Write( xmlDoc, pInventoryElement, "Version", GetVersion() );

	XMLElement* pItemsElement = xmlDoc.NewElement( "Items" );
	pInventoryElement->LinkEndChild( pItemsElement );

	for ( auto& inventoryEntry : m_Items )
	{
		ItemData& itemData = inventoryEntry.second;

		XMLElement* pItemElement = xmlDoc.NewElement( "Item" );
		pItemsElement->LinkEndChild( pItemElement );

		pItemElement->SetText( inventoryEntry.first.c_str() );
		pItemElement->SetAttribute( "quantity", itemData.quantity );
		pItemElement->SetAttribute( "cached", itemData.cached );
	}

	bool result = true;
	result &= m_HexGrid.Write( xmlDoc, pInventoryElement );

	return result;
}

bool Inventory::Read( tinyxml2::XMLElement* pRootElement )
{
	int version = 1;

	for ( tinyxml2::XMLElement* pElement = pRootElement->FirstChildElement(); pElement != nullptr; pElement = pElement->NextSiblingElement() ) 
	{
		Xml::Serialise( pElement, "Version", version );

		if ( std::string( pElement->Value() ) == "Items" )
		{
			ReadItems( pElement );
		}
		else if ( std::string( pElement->Value() ) == "HexGrid" )
		{
			m_HexGrid.Read( pElement );
		}
	}

	if ( version != GetVersion() )
	{
		UpgradeFromVersion( version );
	}

	bool state = m_HexGrid.GetUsedSlots() > 0;
	return state;
}

void Inventory::UpgradeFromVersion( int version )
{
    Genesis::Log::Info() << "Inventory::UpgradeFromVersion(): " << version << " -> " << GetVersion();
}

bool Inventory::ReadItems( tinyxml2::XMLElement* pRootElement )
{
	for ( tinyxml2::XMLElement* pElement = pRootElement->FirstChildElement(); pElement != nullptr; pElement = pElement->NextSiblingElement() ) 
	{
		if ( std::string( pElement->Value() ) == "Item" )
		{
			std::string moduleName = pElement->GetText();
			int quantity = 0;
			int cached = 0;
			pElement->QueryIntAttribute( "quantity", &quantity );
			pElement->QueryIntAttribute( "cached", &cached );
			AddModule( moduleName, quantity, cached );
		}
	}

	return true;
}

// Makes any cached modules permanent, meaning they can no longer be lost if the ship is destroyed.
void Inventory::ClearCachedModules()
{
	int cachedModulesCleared = 0;
	for ( auto& inventoryEntry : m_Items )
	{
		ItemData& itemData = inventoryEntry.second;
		if ( itemData.cached > 0 )
		{
            Genesis::Log::Info() << "Module made permanent: " << inventoryEntry.first << "(" << itemData.cached << ")";
			itemData.cached = 0;
			cachedModulesCleared++;
		}
	}
    Genesis::Log::Info() << cachedModulesCleared << " modules made permanent.";
}

// Removes any temporary modules.
void Inventory::DeductCachedModules()
{
	int modulesLost = 0;
	for ( auto& inventoryEntry : m_Items )
	{
		ItemData& itemData = inventoryEntry.second;
		if ( itemData.cached > 0 )
		{
            Genesis::Log::Info() << "Module lost: " << inventoryEntry.first << "(" << itemData.cached << ")";
			itemData.quantity -= itemData.cached;
			itemData.cached = 0;
		}
	}
    Genesis::Log::Info() << modulesLost << " modules lost.";
}

}