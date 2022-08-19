// Copyright 2018 Pedro Nunes
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

namespace Nullscape
{

/////////////////////////////////////////////////////////////////////
// RadarIcon
/////////////////////////////////////////////////////////////////////

inline RadarIcon::RadarIcon( Genesis::Gui::Atlas::Id atlasIndex, float x, float y, float scale ) : 
m_AtlasIndex( atlasIndex ), 
m_X( x ), 
m_Y( y ),
m_Scale( scale )
{

};

inline Genesis::Gui::Atlas::Id RadarIcon::GetAtlasIndex() const
{
	return m_AtlasIndex;
}

inline float RadarIcon::GetX() const
{
	return m_X;
}

inline float RadarIcon::GetY() const
{
	return m_Y;
}

inline float RadarIcon::GetScale() const
{
	return m_Scale;
}

}
