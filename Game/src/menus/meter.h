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

#include <vector>
#include <gui/gui.h>

namespace Hyperscape
{

typedef std::vector<Genesis::Gui::Panel*> PanelVector;
	
class Meter : public Genesis::Gui::Panel
{
public:
                    Meter();
	void			Init( int initialValue, int maxValue );
	void			SetValue( int value );
    void            SetPipColour( const Genesis::Colour& colour );
    void            SetPipColour( float r, float g, float b, float a );

protected:
	PanelVector		m_Panels;
    Genesis::Colour m_PipColour;
};

}