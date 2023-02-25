#include <sstream>
#include "xmlaux.h"

namespace Xml
{

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Nullscape::ModuleRarity& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Nullscape;

	static const std::string rarity[ static_cast<size_t>( ModuleRarity::Count ) ] = 
	{
		"Trash",
		"Common",
		"Uncommon",
		"Rare",
		"Artifact",
		"Legendary"
	};

	for ( int i = 0; i < static_cast<int>( ModuleRarity::Count ); ++i )
	{
		if ( rarity[ i ] == pElement->GetText() )
		{
			value = static_cast<ModuleRarity>( i );
			return true;
		}
	}

	return false;
}

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Nullscape::WeaponBehaviour& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Nullscape;

	std::string text( pElement->GetText() );
	if ( text == "Fixed" )
	{
		value = WeaponBehaviour::Fixed;
		return true;
	}
	else if ( text == "Turret" )
	{
		value = WeaponBehaviour::Turret;
		return true;
	}

	return false;
}

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Nullscape::TowerBonus& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Nullscape;

	std::string text( pElement->GetText() );
	if ( text == "None" )
	{
		value = TowerBonus::None;
		return true;
	}
	else if ( text == "Damage" )
	{
		value = TowerBonus::Damage;
		return true;
	}
	else if ( text == "Movement" )
	{
		value = TowerBonus::Movement;
		return true;
	}
	else if ( text == "Shields" )
	{
		value = TowerBonus::Shields;
		return true;
	}
	else if ( text == "Sensors" )
	{
		value = TowerBonus::Sensors;
		return true;
	}
	else if ( text == "HyperspaceImmunity" )
	{
		value = TowerBonus::HyperspaceImmunity;
		return true;
	}
	else if ( text == "Ramming" )
	{
		value = TowerBonus::Ramming;
		return true;
	}

	return false;
}

void Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement, const std::string& elementName, const std::string& content )
{
	tinyxml2::XMLElement* pElement = xmlDoc.NewElement( elementName.c_str() );
	pElement->SetText( content.c_str() );
	pRootElement->InsertEndChild( pElement );
}

void Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement, const std::string& elementName, int content )
{
	Write( xmlDoc, pRootElement, elementName, std::to_string( content ) );
}

void Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement, const std::string& elementName, float content )
{
	Write( xmlDoc, pRootElement, elementName, std::to_string( content ) );
}

void Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement, const std::string& elementName, bool content )
{
	Write( xmlDoc, pRootElement, elementName, content ? std::string("true") : std::string("false") );
}

}