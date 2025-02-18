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

#pragma once

#include <string>
#include <map>
#include <vector>

// clang-format off
#include <externalheadersbegin.hpp>
#include <tinyxml2.h>
#include <externalheadersend.hpp>
// clang-format on

#include <colour.h>

#include "ship/hexgrid.h"

namespace Genesis
{

class ResourceSound;

}

namespace Hyperscape
{

class Module;
class ModuleInfo;
class Ship;

typedef HexGrid< ModuleInfo* > ModuleInfoHexGrid;
typedef std::vector< ModuleInfo* > ModuleInfoVector;

enum class ModuleType
{
	Invalid = -1,
	Engine  =  0,
	Armour,
	Shield,
	Weapon,
	Reactor,
	Addon,
	Tower,

	Count
};

enum class ModuleRarity
{
	Trash = 0,
	Common,
	Uncommon,
	Rare,
	Artifact,
	Legendary,

	Count
};

Genesis::Colour ModuleRarityToColour( ModuleRarity rarity );


///////////////////////////////////////////////////////////////////////////////
// ModuleInfoManager
// Keeps a map of all the Modules that can be equipped in a ship.
// The data from the ModuleInfoManager remains valid from the start of the game 
// until it is closed.
// The modules are loaded from all the XML files in Data/Xml/Modules/
///////////////////////////////////////////////////////////////////////////////

typedef std::map< std::string, ModuleInfo* > ModuleMap;

class ModuleInfoManager
{
public:
	ModuleInfoManager();
	~ModuleInfoManager();

	ModuleInfo* GetModuleByName( const std::string& str ) const;
	ModuleInfoVector GetAllModules() const;

private:
	ModuleType StringToModuleType( const std::string& str ) const;
	ModuleInfo* CreateModuleInfo( ModuleType moduleType, tinyxml2::XMLElement* pElement );
	ModuleMap m_Modules;
};


///////////////////////////////////////////////////////////////////////////////
// ModuleInfo
// A ModuleInfo contains the base information for a module that can be 
// installed into a ship.
///////////////////////////////////////////////////////////////////////////////

class ModuleInfo
{
public:
							ModuleInfo( tinyxml2::XMLElement* pElement );
	virtual					~ModuleInfo() {};

	const std::string&		GetName()			const { return m_Name; }
	const std::string&		GetFullName()		const { return m_FullName.empty() ? m_Name : m_FullName; }
	const std::string&		GetDescription()	const { return m_Description; }
	ModuleType				GetType()			const { return m_Type; }
	float					GetActivationCost() const { return m_ActivationCost; }
	const std::string&		GetIcon()			const { return m_Icon; }
	const std::string&		GetModel()			const { return m_Model; }
	ModuleRarity			GetRarity()			const { return m_Rarity; }
	const Genesis::Colour&	GetOverlayColour()	const { return m_OverlayColour; }
    const std::string&      GetContextualTip()  const { return m_ContextualTip; }
    float					GetHealth( const Ship* pShip ) const;

	virtual Module*			CreateModule()		= 0;

protected:
	std::string				m_Name;				// Name of the module. Must be unique.
	std::string				m_FullName;			// Name used for displaying the item in the inventory (optional, will fallback to m_Name)
	std::string				m_Description;		// Description (optional)
	std::string				m_Icon;				// Icon that shows up in the hotbar
	ModuleType				m_Type;				// Type of the module
	float					m_ActivationCost;	// Base energy cost of activating this module
	std::string				m_Model;			// Model that represents this module on ships that render modules individually
	float					m_Health;			// Amount of damage this module can receive before being disabled
	ModuleRarity			m_Rarity;			// Module rarity for loot dropping purposes
	Genesis::Colour			m_OverlayColour;	// Colour of the Ikeda overlay
    std::string             m_ContextualTip;    // Tag to use with the contextual tip system
};


///////////////////////////////////////////////////////////////////////////////
// EngineInfo
///////////////////////////////////////////////////////////////////////////////

enum class EngineBonus
{
	None,
	Stabilised,
	EMPResistant,
	ReducesHyperspaceChargeup
};

class EngineInfo: public ModuleInfo
{
public:
						EngineInfo( tinyxml2::XMLElement* pElement );
	virtual				~EngineInfo() {};

	virtual Module*		CreateModule();

	float				GetThrust()	const		{ return m_Thrust; }
	float				GetTorque() const		{ return m_Torque; }
	bool				HasBonus( EngineBonus bonus ) const;

private:
	float				m_Thrust;
	float				m_Torque;
	bool				m_IsStabilised;
	bool				m_IsEMPResistant;
	bool				m_ReducesHyperspaceChargeup;
};


///////////////////////////////////////////////////////////////////////////////
// SensorInfo
///////////////////////////////////////////////////////////////////////////////

class SensorInfo: public ModuleInfo
{
public:
						SensorInfo( tinyxml2::XMLElement* pElement );
	virtual				~SensorInfo() {};
	virtual Module*		CreateModule();
};


///////////////////////////////////////////////////////////////////////////////
// ArmourInfo
///////////////////////////////////////////////////////////////////////////////

class ArmourInfo: public ModuleInfo
{
public:
						ArmourInfo( tinyxml2::XMLElement* pElement );
	virtual				~ArmourInfo() {};
	virtual Module*		CreateModule();

	float				GetMassMultiplier( Ship* pShip ) const;
	inline bool			IsRammingProw()			const { return m_RammingProw; }
	inline bool			IsRegenerative()		const { return m_Regenerative; }
	inline float		GetKineticResistance()	const { return m_KineticResistance; }
	inline float		GetEnergyResistance()	const { return m_EnergyResistance; }

private:
	float				m_MassMultiplier;		// Mass added to the ship by equipping this armour module
	float				m_KineticResistance;	// Chance to block damage against kinetic damage (projectiles)
	float				m_EnergyResistance;		// Flat damage % decrease against energy damage (missiles / proton / lances / ion)
	bool				m_Regenerative;			// Does this armour regenerate its health over time?
	bool				m_RammingProw;			// Deals extra damage during ramming, while taking very little damage.
};


///////////////////////////////////////////////////////////////////////////////
// ShieldInfo
///////////////////////////////////////////////////////////////////////////////

class ShieldInfo: public ModuleInfo
{
public:
						ShieldInfo( tinyxml2::XMLElement* pElement );
	virtual				~ShieldInfo() {};
	virtual Module*		CreateModule();

	float				GetCapacity()		const { return m_Capacity; }
	float				GetPeakRecharge()	const { return m_PeakRecharge; }
	float				GetEnergyUsage( Ship* pShip ) const;
	bool				IsOvertuned()		const { return m_Overtuned; }

protected:
	float				m_Capacity;
	float				m_PeakRecharge;
	float				m_EnergyUsage;
	bool				m_Overtuned;
};


///////////////////////////////////////////////////////////////////////////////
// WeaponInfo
///////////////////////////////////////////////////////////////////////////////

enum class WeaponBehaviour
{
	Invalid = -1,
	Fixed,
	Turret
};

enum class WeaponSystem
{
	Invalid = -1,
	Projectile,
	Missile,
	Rocket,
	Torpedo,
	Antiproton,
	Lance,
	Ion,
	Universal,

	Count
};

enum class DamageType
{
	Kinetic,
	Energy,
    TrueDamage,
    EMP,
	Collision
};

class WeaponInfo: public ModuleInfo
{
public:
								WeaponInfo( tinyxml2::XMLElement* pElement );
	virtual						~WeaponInfo() {};
	virtual Module*				CreateModule();

	float						GetActivationCost( Ship* pShip ) const;
	WeaponBehaviour				GetBehaviour()			const { return m_Behaviour; }
	WeaponSystem				GetSystem()				const { return m_System; }
	const std::string&			GetWeaponModel()		const { return m_WeaponModel; }
	DamageType					GetDamageType()			const { return m_DamageType; }

	float						GetRateOfFire( Ship* pShip ) const;
	float						GetDamage()				const { return m_Damage; }
	float						GetDPS( Ship* pShip )	const { return GetDamage() * GetRateOfFire( pShip ); }
	int							GetBurst()				const { return m_Burst; }
	float						GetRayLength()			const { return m_RayLength; }
	float						GetSpeed()				const { return m_Speed; }
	float						GetRange( Ship* pShip )	const;
	float						GetTracking()			const { return m_Tracking; }
	bool						GetIsSwarm()			const { return m_IsSwarm; }
	float						GetBeamWidth()			const { return m_BeamWidth; }
	float						GetBeamLifetime()		const { return m_BeamLifetime; }
	const Genesis::Colour&		GetBeamColour()			const { return m_BeamColour; }
	float						GetMuzzleflashScale()	const { return m_MuzzleflashScale; }
	const Genesis::Colour&		GetMuzzleflashColour()	const { return m_MuzzleflashColour; }

	Genesis::ResourceSound*		GetOnFireSFX()			const { return m_pOnFireSFX; }
	Genesis::ResourceSound*		GetOnHitSFX()			const {	return m_pOnHitSFX; }
	float						GetOnHitSFXDistance()	const { return m_OnHitSFXDistance; }

private:
	WeaponBehaviour				m_Behaviour;
	WeaponSystem				m_System;
	std::string					m_WeaponModel;
	float						m_Rof;
	float						m_Damage;
	int							m_Burst;
	float						m_RayLength;
	float						m_Speed;
	float						m_Range;
	float						m_Tracking;
	bool						m_IsSwarm;
	float						m_BeamWidth;
	float						m_BeamLifetime;
	Genesis::Colour				m_BeamColour;
	Genesis::ResourceSound*		m_pOnFireSFX;
	Genesis::ResourceSound*		m_pOnHitSFX;
	float						m_OnHitSFXDistance;
	DamageType					m_DamageType;
	float						m_MuzzleflashScale;
	Genesis::Colour				m_MuzzleflashColour;
};


///////////////////////////////////////////////////////////////////////////////
// ReactorInfo
///////////////////////////////////////////////////////////////////////////////

class ReactorInfo: public ModuleInfo
{
public:
						ReactorInfo( tinyxml2::XMLElement* pElement );
	virtual				~ReactorInfo() {};
	virtual Module*		CreateModule();

	float				GetCapacity()		const { return m_Capacity; }
	float				GetRechargeRate()	const { return m_RechargeRate; }

protected:
	float				m_Capacity;
	float				m_RechargeRate;
};


///////////////////////////////////////////////////////////////////////////////
// AddonInfo
///////////////////////////////////////////////////////////////////////////////

enum class AddonCategory
{
	Invalid = -1,
	ModuleRepairer,
	HangarBay,
	DroneBay,
	MissileInterceptor,
	PhaseBarrier,
	FuelInjector,
	EngineDisruptor,
	ParticleAccelerator,
	QuantumStateAlternator
};

enum class AddonActivationType
{
	Invalid,
	Trigger,
	Toggle
};

class AddonInfo: public ModuleInfo
{
public:
						AddonInfo( tinyxml2::XMLElement* pElement );
	virtual				~AddonInfo() {};
	virtual Module*		CreateModule();

	AddonCategory		GetCategory()		const { return m_Category; }
	AddonActivationType GetType()			const { return m_ActivationType; }
	float				GetCooldown()		const { return m_Cooldown; }
	const std::string&	GetParameter()		const { return m_Parameter; }

protected:
	AddonCategory		m_Category;
	AddonActivationType m_ActivationType;
	float				m_Cooldown;
	std::string			m_Parameter;
};


///////////////////////////////////////////////////////////////////////////////
// TowerInfo
///////////////////////////////////////////////////////////////////////////////

enum class TowerBonus
{
	None,
	Damage,
	Movement,
	Shields,
	Sensors,
	HyperspaceImmunity,
	Ramming
};

class TowerInfo: public ModuleInfo
{
public:
						TowerInfo( tinyxml2::XMLElement* pElement );
	virtual				~TowerInfo() {};
	virtual Module*		CreateModule();

	TowerBonus			GetBonusType() const			{ return m_BonusType; }
	float				GetBonusMagnitude() const		{ return m_BonusMagnitude; }
	const std::string&	GetShortDescription() const		{ return m_ShortDescription; }

private:
	TowerBonus			m_BonusType;
	float				m_BonusMagnitude;
	std::string			m_ShortDescription;
};

}
