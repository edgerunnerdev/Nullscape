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

#pragma once

#include <vector>

#include <colour.h>
#include <resources/resourceshader.hpp>
#include <scene/layer.h>
#include <scene/sceneobject.h>

#include "menus/sectordetails.h"
#include "misc/mathaux.h"
#include "requests/requestgoal.h"
#include "ui/window.h"


namespace Genesis
{
	class Layer;
	class ResourceImage;
	class VertexBuffer;

	namespace Gui
	{
		class Text;
	}
}

namespace Hexterminate
{

class Galaxy;
class SectorInfo;
class SectorDetails;
class GalaxyWindow;
using GalaxyWindowSharedPtr = std::shared_ptr<GalaxyWindow>;


/////////////////////////////////////////////////////////////////////
// SectorDrawInfo
/////////////////////////////////////////////////////////////////////

struct SectorDrawInfo
{
	SectorDrawInfo()
	{
		x = 0;
		y = 0;
	};

	SectorDrawInfo( int _x, int _y )
	{
		x = _x;
		y = _y;
	};

	Genesis::Colour colour;
	int x, y;
};

typedef std::vector< SectorDrawInfo > SectorDrawInfoVector;


/////////////////////////////////////////////////////////////////////
// GalaxyRep
/////////////////////////////////////////////////////////////////////

class GalaxyRep: public Genesis::SceneObject
{
public:
								GalaxyRep( Galaxy* pGalaxy );
	virtual						~GalaxyRep();

	void						Initialise();
	void						OnGalaxyPopulated();
	void						OnGalaxyReset();
	void						RemoveFromScene();

	virtual void				Update( float delta );
	virtual void				Render();

	void						Show( bool state );
	inline bool					IsVisible() const									{ return m_Show; }
	inline void					GetOffset( float& offsetX, float& offsetY ) const	{ offsetX = m_OffsetX; offsetY = m_OffsetY; }
	Math::FPoint2				GetOffset() const									{ return Math::FPoint2( m_OffsetX, m_OffsetY ); }

	GalaxyWindow*				GetGalaxyWindow() const;

private:
	void						UpdateInput();
	void						UpdateDrawInfo();
	void						UpdateGoalDrawInfo();

	void						DrawBackground();
	void						DrawSectors( SectorDrawInfoVector& drawInfoVec, Genesis::ResourceShader* pShader, Genesis::ShaderUniformInstances* pShaderUniforms, bool useFactionColour );
	void						DrawSectorsThreatRatings();
	void						DrawHomeworldSectors();
	void						DrawGrid();
	void						DrawGoals();

	void						FocusOnPlayerFleet();	// Sets the offsets needed to draw the galaxy correctly, while trying to player's fleet centered.
	void						SetHoverSector();		// Decides which sector lies underneath the mouse pointer, if any. Result can be nullptr.

	void						OnLeftMouseButtonDown();

	Galaxy*						m_pGalaxy;
	bool						m_Show;

	Genesis::ResourceShader*	m_pBackgroundShader;
	Genesis::VertexBuffer*		m_pBackgroundVB;

	Genesis::ResourceShader*	m_pSectorShader;
	Genesis::ResourceShader*	m_pSectorShipyardShader;
	Genesis::ResourceShader*	m_pSectorProbeShader;
	Genesis::ResourceShader*	m_pSectorStarfortShader;
	Genesis::ResourceShader*	m_pSectorInhibitorShader;
	Genesis::ResourceShader*	m_pSectorHomeworldShader;
	Genesis::VertexBuffer*		m_pSectorVB;
	Genesis::VertexBuffer*		m_pSectorInhibitorVB;
	Genesis::VertexBuffer*		m_pSectorHomeworldVB;

	Genesis::ResourceShader*	m_pSectorCrossShader;
	Genesis::VertexBuffer*		m_pSectorCrossVB;
    Genesis::ShaderUniformInstances m_SectorCrossUniforms;

	Genesis::ShaderUniformSharedPtr	m_pSectorHomeworldDiffuseSampler;
	std::array< Genesis::ResourceImage*, (int)FactionId::Count > m_HomeworldImages;

	Genesis::ResourceShader*			m_pSectorThreatShader;
	std::array< Genesis::ShaderUniformInstances, static_cast<size_t>( ThreatRating::Count ) > m_pSectorThreatUniforms;

	Genesis::ResourceShader*			m_pGoalTargetShader;
	Genesis::VertexBuffer*		m_pGoalTargetVB;

	Genesis::LayerSharedPtr		m_pLayer;

	float						m_OffsetX;
	float						m_OffsetY;

	SectorInfo*					m_pHoverSector;
	SectorDetails*				m_pSectorDetails;

	bool						m_ExitMenu;

	SectorDrawInfoVector		m_SectorDrawInfo;
	SectorDrawInfoVector		m_SectorDrawInfoShipyard;
	SectorDrawInfoVector		m_SectorDrawInfoProbes;
	SectorDrawInfoVector		m_SectorDrawInfoStarforts;
	SectorDrawInfoVector		m_SectorDrawInfoInhibitors;

	GalaxyWindowSharedPtr		m_pGalaxyWindow;
	Genesis::InputCallbackToken m_LeftMouseButtonDownToken;

	bool						m_InputPending;
	uint32_t					m_InputTimer;
};

}