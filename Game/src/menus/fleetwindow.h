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

#include <vector>

#include <gui/gui.h>

#include "ui/types.fwd.h"
#include "ui/window.h"

namespace Hyperscape
{

class ShipInfo;

/////////////////////////////////////////////////////////////////////
// FleetWindow
/////////////////////////////////////////////////////////////////////

class FleetWindow : public UI::Window
{
public:
	FleetWindow();
	virtual ~FleetWindow();

	void Show(bool state);

private:
	void PopulateFleetShips();
	void PopulateRequisitionShips();
	void RefreshFleetShips();
	void RefreshRequisitionShips();
	bool CanRequisitionShip( const ShipInfo* pShipInfo ) const;
	bool HasSufficientInfluence( const ShipInfo* pShipInfo ) const;
	bool HasNecessaryPerk( const ShipInfo* pShipInfo ) const;
	void RequisitionShip( const ShipInfo* pShipInfo );

	UI::PanelSharedPtr m_pFleetPanel;
	UI::TextSharedPtr m_pFleetSubtitle;
	UI::PanelSharedPtr m_pRequisitionShipsPanel;
	UI::TextSharedPtr m_pRequisitionShipsSubtitle;
	UI::ScrollingElementSharedPtr m_pRequisitionShipsArea;
	
	struct FleetShipInfo
	{
		FleetShipInfo()
		{
			isPlayerShip = false;
			pShipInfo = nullptr;
		}

		bool isPlayerShip;
		const ShipInfo* pShipInfo;
		UI::PanelSharedPtr pPanel;
		UI::TextSharedPtr pNameText;
		UI::TextSharedPtr pCategoryText;
		UI::ButtonSharedPtr pReturnButton;
		UI::ImageSharedPtr pUnavailableIcon;
		UI::TextSharedPtr pUnavailableText;
	};
	std::vector<FleetShipInfo> m_FleetShipInfos;

	struct RequisitionShipInfo
	{
		const ShipInfo* pShipInfo;
		UI::PanelSharedPtr pPanel;
		UI::ImageSharedPtr pBackground;
		UI::TextSharedPtr pTitle;

		UI::ImageSharedPtr pWeaponsIcon;
		UI::TextSharedPtr pWeaponsText;
		UI::ImageSharedPtr pDefenseIcon;
		UI::TextSharedPtr pDefenseText;
		UI::ImageSharedPtr pInfluenceIcon;
		UI::TextSharedPtr pInfluenceText;
		UI::ImageSharedPtr pPerkIcon;
		UI::TextSharedPtr pPerkText;

		UI::ButtonSharedPtr pRequisitionButton;
	};
	std::vector<RequisitionShipInfo> m_RequisitionShipInfos;
	bool m_RequisitionTextInitialized;
};

}