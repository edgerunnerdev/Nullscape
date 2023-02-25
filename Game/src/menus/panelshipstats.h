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

#pragma once

#include <gui/gui.h>
#include "ship/moduleinfo.h"

namespace Hyperscape
{

class Table;
class TableRow;

// Displays the stats for the player ship. 
class PanelShipStats
{
public:
							PanelShipStats();
	virtual					~PanelShipStats();
	void					Show( bool state );
	void					OnShipConfigurationChanged();

private:
	void					AddEmptyRow();
	void					AddTitleRow( const std::string& title );

	void					AddWeaponryRows();
	void					AddShieldRows();
	void					AddEnergyRows();

	void					UpdateWeaponryStats();
	void					UpdateShieldStats();
	void					UpdateEnergyStats();

	float					CalculateBonusMultiplier( TowerBonus towerBonus ) const;

	Genesis::Gui::Panel*	m_pMainPanel;
	Genesis::Gui::Text*		m_pTitle;

	Table*					m_pTable;

	TableRow*				m_pWeaponryTurretRow;
	TableRow*				m_pWeaponryFixedRow;
	TableRow*				m_pShieldCapacityRow;
	TableRow*				m_pShieldRechargeRow;
	TableRow*				m_pShieldEfficiencyRow;
	TableRow*				m_pEnergyCapacityRow;
	TableRow*				m_pEnergyRechargeRow;
	TableRow*				m_pEnergyUsageRow;

	float					m_EnergyUsage;
};

}