// Copyright 2015 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#include <genesis.h>
#include <configuration.h>
#include "menus/eva.h"
#include "menus/panelshipstats.h"
#include "menus/table.h"
#include "menus/tablerow.h"
#include "sector/sector.h"
#include "ship/module.h"
#include "ship/moduleinfo.h"
#include "ship/shield.h"
#include "player.h"
#include "hexterminate.h"
#include "stringaux.h"

namespace Hexterminate
{

PanelShipStats::PanelShipStats() :
m_pMainPanel( nullptr ),
m_pTitle( nullptr ),
m_pTable( nullptr ),
m_pWeaponryTurretRow( nullptr ),
m_pWeaponryFixedRow( nullptr ),
m_pShieldCapacityRow( nullptr ),
m_pShieldRechargeRow( nullptr ),
m_pShieldEfficiencyRow( nullptr ),
m_pEnergyCapacityRow( nullptr ),
m_pEnergyRechargeRow( nullptr ),
m_pEnergyUsageRow( nullptr),
m_EnergyUsage( 0.0f )
{
	Genesis::Gui::GuiManager* pGuiManager = Genesis::FrameWork::GetGuiManager();

	const float panelWidth = 256.0f;
	const float screenWidth = static_cast<float>( Genesis::Configuration::GetScreenWidth() );
	const float outerBorder = 8.0f;
	const float innerBorder = 8.0f;

	m_pMainPanel = new Genesis::Gui::Panel();
	m_pMainPanel->SetPosition( screenWidth - panelWidth - outerBorder, outerBorder );
	m_pMainPanel->SetColour( 0.0f, 0.0f, 0.0f, 0.25f );
	m_pMainPanel->SetBorderColour( 1.0f, 1.0f, 1.0f, 0.25f );
	m_pMainPanel->SetBorderMode( Genesis::Gui::PANEL_BORDER_ALL );
	pGuiManager->AddElement( m_pMainPanel );

	m_pTitle = new Genesis::Gui::Text();
	m_pTitle->SetSize( 256.0f, 16.0f );
	m_pTitle->SetPosition( innerBorder, innerBorder );
	m_pTitle->SetColour( EVA_TEXT_COLOUR );
	m_pTitle->SetFont( EVA_FONT );
	m_pTitle->SetText( "Shipyard > Overview" );
	m_pMainPanel->AddElement( m_pTitle );
	
	m_pTable = new Table();
	m_pTable->SetPosition( innerBorder, 48.0f );
	
	AddWeaponryRows();
	AddEmptyRow();
	AddShieldRows();
	AddEmptyRow();
	AddEnergyRows();

	// Resize window to fit all the rows we need
	const float tableHeight = m_pTable->GetRowCount() * m_pTable->GetRowHeight();
	m_pTable->SetSize( panelWidth - innerBorder * 2.0f, tableHeight );
	m_pMainPanel->SetSize( panelWidth, 48.0f + tableHeight + 8.0f );

	m_pMainPanel->AddElement( m_pTable );

	OnShipConfigurationChanged();
}

PanelShipStats::~PanelShipStats()
{
	Genesis::Gui::GuiManager* pGuiManager = Genesis::FrameWork::GetGuiManager();
	if ( pGuiManager != nullptr )
	{
		pGuiManager->RemoveElement( m_pMainPanel );
	}
}

void PanelShipStats::AddEmptyRow()
{
	TableRow* pEmptyRow = new TableRow();
	pEmptyRow->Add( "" );
	pEmptyRow->Add( "" );
	m_pTable->AddRow( pEmptyRow );
}

void PanelShipStats::AddTitleRow( const std::string& title )
{
	TableRow* pTitleRow = new TableRow();
	pTitleRow->Add( title );
	pTitleRow->Add( "" );
	m_pTable->AddRow( pTitleRow );
}

void PanelShipStats::AddWeaponryRows()
{
	AddTitleRow( "Weaponry" );

	m_pWeaponryTurretRow = new TableRow();
	m_pWeaponryTurretRow->Add( "- Turrets:" );
	m_pWeaponryTurretRow->Add( "0 dps" );
	m_pTable->AddRow( m_pWeaponryTurretRow );

	m_pWeaponryFixedRow = new TableRow();
	m_pWeaponryFixedRow->Add( "- Fixed:" );
	m_pWeaponryFixedRow->Add( "0 dps" );
	m_pTable->AddRow( m_pWeaponryFixedRow );
}

void PanelShipStats::AddShieldRows()
{
	AddTitleRow( "Shield" );

	m_pShieldCapacityRow = new TableRow();
	m_pShieldCapacityRow->Add( "- Capacity:" );
	m_pShieldCapacityRow->Add( "0 u" );
	m_pTable->AddRow( m_pShieldCapacityRow );

	m_pShieldRechargeRow = new TableRow();
	m_pShieldRechargeRow->Add( "- Recharge:" );
	m_pShieldRechargeRow->Add( "0 u/s" );
	m_pTable->AddRow( m_pShieldRechargeRow );

	m_pShieldEfficiencyRow = new TableRow();
	m_pShieldEfficiencyRow->Add( "- Efficiency:" );
	m_pShieldEfficiencyRow->Add( "100%" );
	m_pTable->AddRow( m_pShieldEfficiencyRow );
}

void PanelShipStats::AddEnergyRows()
{
	AddTitleRow( "Energy" );

	m_pEnergyCapacityRow = new TableRow();
	m_pEnergyCapacityRow->Add( "- Capacity:" );
	m_pEnergyCapacityRow->Add( "0 u" );
	m_pTable->AddRow( m_pEnergyCapacityRow );
	
	m_pEnergyRechargeRow = new TableRow();
	m_pEnergyRechargeRow->Add( "- Recharge:" );
	m_pEnergyRechargeRow->Add( "0 u/s" );
	m_pTable->AddRow( m_pEnergyRechargeRow );

	m_pEnergyUsageRow = new TableRow();
	m_pEnergyUsageRow->Add( "- Usage:" );
	m_pEnergyUsageRow->Add( "0 u/s" );
	m_pTable->AddRow( m_pEnergyUsageRow );
}

void PanelShipStats::OnShipConfigurationChanged()
{
	m_EnergyUsage = 0.0f; // Reset the energy usage and update it if needed inside the various Update*Stats()

	UpdateWeaponryStats();
	UpdateShieldStats();
	UpdateEnergyStats();

	m_pTable->NotifyContentUpdated();
}

void PanelShipStats::UpdateWeaponryStats()
{
	float fixedWeaponDPS = 0.0f;
	float turretWeaponDPS = 0.0f;

	Ship* pShip = g_pGame->GetPlayer()->GetShip();
	const WeaponModuleList& weaponModules = pShip->GetWeaponModules();
	for ( auto& pWeaponModule : weaponModules )
	{
		WeaponInfo* pWeaponInfo = static_cast<WeaponInfo*>( pWeaponModule->GetModuleInfo() );
		if ( pWeaponInfo->GetBehaviour() == WeaponBehaviour::Fixed )
		{
			fixedWeaponDPS += pWeaponInfo->GetDPS( pShip );
		}
		else if ( pWeaponInfo->GetBehaviour() == WeaponBehaviour::Turret )
		{
			turretWeaponDPS += pWeaponInfo->GetDPS( pShip );
		}
	}

	float bonusMultiplier = CalculateBonusMultiplier( TowerBonus::Damage );
	fixedWeaponDPS *= bonusMultiplier;
	turretWeaponDPS *= bonusMultiplier;

	m_pWeaponryFixedRow->Set( 1, ToString( floor( fixedWeaponDPS ) ) + " dps" );
	m_pWeaponryTurretRow->Set( 1, ToString( floor( turretWeaponDPS ) ) + " dps" );
}

void PanelShipStats::UpdateShieldStats()
{
	float shieldCapacity = 0.0f;
	float shieldRecharge = 0.0f;

	Ship* pShip = g_pGame->GetPlayer()->GetShip();
	const ShieldModuleList& shieldModules = pShip->GetShieldModules();
	float shieldEfficiency = Shield::CalculateEfficiency( shieldModules );

	for ( auto& pShieldModule : shieldModules )
	{
		ShieldInfo* pShieldInfo = static_cast<ShieldInfo*>( pShieldModule->GetModuleInfo() );
		shieldCapacity += pShieldInfo->GetCapacity() * shieldEfficiency;
		shieldRecharge += pShieldInfo->GetPeakRecharge() * shieldEfficiency;
		m_EnergyUsage += pShieldInfo->GetEnergyUsage( pShip );
	}

	float bonusMultiplier = CalculateBonusMultiplier( TowerBonus::Shields );
	shieldCapacity *= bonusMultiplier;
	shieldRecharge *= bonusMultiplier;

	m_pShieldCapacityRow->Set( 1, ToString( floor( shieldCapacity ) ) + " u" );
	m_pShieldRechargeRow->Set( 1, ToString( floor( shieldRecharge ) ) + " u/s" );
	m_pShieldEfficiencyRow->Set( 1, ToString( floor( shieldEfficiency * 100.0f ) ) + "%" );
}

void PanelShipStats::UpdateEnergyStats()
{
	float energyCapacity = 0.0f;
	float energyRecharge = 0.0f;

	Ship* pShip = g_pGame->GetPlayer()->GetShip();
	const ReactorModuleList& reactorModules = pShip->GetReactorModules();
	for ( auto& pReactorModule : reactorModules )
	{
		ReactorInfo* pReactorInfo = static_cast<ReactorInfo*>( pReactorModule->GetModuleInfo() );
		energyCapacity += pReactorInfo->GetCapacity();
		energyRecharge += pReactorInfo->GetRechargeRate();
	}

	m_pEnergyCapacityRow->Set( 1, ToString( floor( energyCapacity ) ) + " u" );
	m_pEnergyRechargeRow->Set( 1, ToString( floor( energyRecharge ) ) + " u/s" );
	m_pEnergyUsageRow->Set( 1, ToString( floor( m_EnergyUsage ) ) + " u/s" );

	Genesis::Colour clr = ( energyRecharge >= m_EnergyUsage ) ? Genesis::Colour( 1.0f, 1.0f, 1.0f ) : Genesis::Colour( 1.0f, 0.0f, 0.0f );
	m_pEnergyRechargeRow->SetColour( clr );
	m_pEnergyUsageRow->SetColour( clr );
}

float PanelShipStats::CalculateBonusMultiplier( TowerBonus towerBonus ) const
{
	// Usually the bridge bonus is stored in the sector but this doesn't work correctly when
	// we are editting the ship as it is possible to have multiple bridges affecting the same
	// bonus. This creates an invalid ship which can't be undocked but we should still show
	// the correct stats.
	float multiplier = 1.0f;
	
	Ship* pShip = g_pGame->GetPlayer()->GetShip();
	const TowerModuleList& towerModules = pShip->GetTowerModules();
	for ( auto& pTowerModule : towerModules )
	{
		TowerInfo* pTowerInfo = static_cast<TowerInfo*>( pTowerModule->GetModuleInfo() );
		if ( pTowerInfo->GetBonusType() == towerBonus )
		{
			multiplier *= pTowerInfo->GetBonusMagnitude();
		}
	}
	
	return multiplier;
}

void PanelShipStats::Show( bool state )
{
	m_pMainPanel->Show( state );
}

}