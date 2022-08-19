// Copyright 2014 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <gui/gui.h>
#include "eva.h"

namespace Nullscape
{

class AudioDebug
{
public:
	AudioDebug();
	~AudioDebug();
	void Update( float delta );
	void Show( bool state );
	bool IsVisible() const;

private:
	EvaWindow* m_pWindow;
	Genesis::Gui::Text* m_pTextGui;
};

}