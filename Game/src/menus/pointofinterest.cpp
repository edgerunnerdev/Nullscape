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

#include "menus/pointofinterest.h"

#include <configuration.h>
#include <genesis.h>
#include <resources/resourceimage.h>
#include <resources/resourceshader.hpp>
#include <rendersystem.h>
#include <vertexbuffer.h>

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// PointOfInterest
///////////////////////////////////////////////////////////////////////////////

PointOfInterest::PointOfInterest() :
m_pDynamicEnd( nullptr ),
m_pShader( nullptr ),
m_pVertexBuffer( nullptr )
{
	using namespace Genesis;

	m_Colour.Set( 1.0f, 1.0f, 1.0f, 1.0f );

	SetSize( 
		static_cast<float>( Genesis::Configuration::GetScreenWidth() ), 
		static_cast<float>( Genesis::Configuration::GetScreenHeight() ) );

	m_pVertexBuffer = new VertexBuffer( GeometryType::Line, VBO_POSITION | VBO_UV | VBO_COLOUR );
	m_pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/untextured_vertex_coloured.glsl");
}

PointOfInterest::~PointOfInterest()
{
	delete m_pVertexBuffer;
}

void PointOfInterest::Render()
{
	using namespace Genesis;

	Gui::GuiElement::Render();

	const glm::vec2& end = ( m_pDynamicEnd ? m_pDynamicEnd->GetPointOfInterestEnd() : m_StaticEnd );

	PositionData posData;
	posData.push_back( glm::vec3( m_Start.x, m_Start.y, 0.0f ) );
	posData.push_back( glm::vec3( end.x, end.y, 0.0f ) );

	ColourData colourData;
	colourData.push_back( m_Colour.glm() );
	colourData.push_back( m_Colour.glm() );

	m_pVertexBuffer->CopyPositions( posData );
	m_pVertexBuffer->CopyColours( colourData );

	m_pShader->Use();
	m_pVertexBuffer->Draw();
}

}