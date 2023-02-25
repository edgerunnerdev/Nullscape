// Copyright 2016 Pedro Nunes
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
#include "gameevents.h"

namespace Hyperscape
{

class Meter;
class ModuleInfo;

class LootWindow : public GameEventHandler
{
public:
							LootWindow();
							~LootWindow();
	void					Update( float delta );

	virtual	void			HandleGameEvent( GameEvent* pEvent ) override;

private:
    void                    InitialiseSounds();
    void                    Test( float delta );
	float					CalculateIconScaling() const;
	float					CalculateAlpha() const;

	Genesis::Gui::Panel*	m_pMainPanel;
	Genesis::Gui::Text*		m_pModuleName;
	Genesis::Gui::Image*	m_pIcon;
	Genesis::Gui::Image*	m_pWingsLeft;
	Genesis::Gui::Image*	m_pWingsRight;
	Meter*					m_pPerkMeter;
	Genesis::Gui::Text*		m_pPerkAcquired;

	bool					m_Dirty;

    float                   m_PanelWidth;
    float                   m_PanelHeight;
    float                   m_IconSize;

	struct LootElement
	{
		ModuleInfo* pModuleInfo;
		unsigned int quantity;
	};
	typedef std::list< LootElement > LootQueue;
	LootQueue				m_LootQueue;
	float					m_DisplayTimer;

    Genesis::ResourceSound* m_pPerkPointAcquiredSFX;
    Genesis::ResourceSound* m_pPerkProgressSFX;
};

}