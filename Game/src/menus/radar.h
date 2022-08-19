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

#include <vector>
#include <gui/gui.h>
#include <gui/atlas.h>
#include <rendersystem.h>
#include <vertexbuffer.h>

namespace Genesis
{
	class ResourceImage;
	class ResourceShader;
}

namespace Nullscape
{

/////////////////////////////////////////////////////////////////////
// RadarIcon
/////////////////////////////////////////////////////////////////////

class RadarIcon
{
public:
	RadarIcon( Genesis::Gui::Atlas::Id atlasIndex, float x, float y, float scale );
    Genesis::Gui::Atlas::Id GetAtlasIndex() const;
	float GetX() const;
	float GetY() const;
	float GetScale() const;

private:
    Genesis::Gui::Atlas::Id m_AtlasIndex;
	float m_X;
	float m_Y;
	float m_Scale;
};

typedef std::vector< RadarIcon > RadarIconVector;


/////////////////////////////////////////////////////////////////////
// Radar
/////////////////////////////////////////////////////////////////////

class Radar: public Genesis::Gui::GuiElement
{
public:
								Radar();
	virtual						~Radar();
	virtual void				Update( float delta ) override;
	virtual void				Render() override;
	
private:
	void						CachePlayerShipPosition();
	void						AddQuad( Genesis::Gui::Atlas::Id atlasIndex, float x, float y, float scale = 1.0f );
	void						AddShips();
	void						AddShipyard();
	void						AddIcon( Genesis::Gui::Atlas::Id atlasIndex, const glm::vec3& position );

	Genesis::ResourceImage*		m_pTexture;
	Genesis::Gui::Atlas			m_Atlas;
	RadarIconVector				m_Icons;
	
	// Atlas indices
	Genesis::Gui::Atlas::Id		m_BackgroundIdx;
    Genesis::Gui::Atlas::Id		m_BackgroundMaskIdx;
    Genesis::Gui::Atlas::Id		m_FriendlyShipIdx;
    Genesis::Gui::Atlas::Id		m_HostileShipIdx;
    Genesis::Gui::Atlas::Id		m_ShipyardIdx;

	// Rendering
	Genesis::VertexBuffer*		m_pVertexBuffer;
	Genesis::ResourceShader*	m_pShader;
	glm::vec3					m_CachedPlayerShipPosition;
	Genesis::PositionData		m_PositionData;
	Genesis::UVData				m_UVData;
	unsigned int				m_QuadCount;
};

}

#include "radar.inl"
