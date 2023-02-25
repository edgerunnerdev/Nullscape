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
#include "eva.h"

namespace Hyperscape
{

class MusicTitle
{
public:
							MusicTitle();
							~MusicTitle();
	void					Update( float delta );

private:
	Genesis::Gui::Text*		m_pTextGui;
	Genesis::Gui::Image*	m_pIcon;
};

}