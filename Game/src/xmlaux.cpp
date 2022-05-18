#include <sstream>
#include "xmlaux.h"

namespace Xml
{

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::ModuleRarity& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

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

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::WeaponBehaviour& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

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

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::TowerBonus& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

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

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::FleetState& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

	std::string text( pElement->GetText() );
	std::string texts[] = { "Idle", "Engaged", "Moving", "Arrived" };
	FleetState states[] = { FleetState::Idle, FleetState::Engaged, FleetState::Moving, FleetState::Arrived };
	for ( int i = 0; i < 4; ++i )
	{
		if ( text == texts[ i ] )
		{
			value = states[ i ];
			return true;
		}
	}

	return false;
}

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::Difficulty& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

	std::string text( pElement->GetText() );
	std::string texts[] = { "Easy", "Normal", "Hardcore", "Arrived" };
	Difficulty difficulty[] = { Difficulty::Easy, Difficulty::Normal, Difficulty::Hardcore };
	for ( int i = 0; i < 4; ++i )
	{
		if ( text == texts[ i ] )
		{
			value = difficulty[ i ];
			return true;
		}
	}

	return false;
}

bool Serialise( tinyxml2::XMLElement* pElement, const std::string& name, Hexterminate::GameMode& value )
{
	SDL_assert( pElement != nullptr );

	if ( name != pElement->Value() )
		return false;

	using namespace Hexterminate;

	std::string text( pElement->GetText() );
	std::string texts[] = { "Campaign", "Infinite War" };
	GameMode gameMode[] = { GameMode::Campaign, GameMode::InfiniteWar };
	for ( int i = 0; i < 2; ++i )
	{
		if ( text == texts[ i ] )
		{
			value = gameMode[ i ];
			return true;
		}
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

void Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement, const std::string& elementName, Hexterminate::FleetState content )
{
	using namespace Hexterminate;

	if ( content == FleetState::Idle )
		Write( xmlDoc, pRootElement, elementName, std::string( "Idle" ) );
	else if ( content == FleetState::Engaged )
		Write( xmlDoc, pRootElement, elementName, std::string( "Engaged" ) );
	else if ( content == FleetState::Moving )
		Write( xmlDoc, pRootElement, elementName, std::string( "Moving" ) );
	else if ( content == FleetState::Arrived )
		Write( xmlDoc, pRootElement, elementName, std::string( "Arrived" ) );
}

}