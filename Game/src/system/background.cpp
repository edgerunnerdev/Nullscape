// Copyright 2022 Pedro Nunes
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

#include <algorithm>
#include <sstream>

#include <genesis.h>
#include <resources/resourceimage.h>
#include <resources/resourcemodel.h>
#include <rendersystem.h>
#include <math/misc.h>
#include <configuration.h>
#include <shaderuniform.h>
#include <vertexbuffer.h>

#include "system/background.hpp"
#include "hexterminate.h"
#include "player.h"

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Background
///////////////////////////////////////////////////////////////////////////////

Background::Background(const std::string& seed) :
m_pShader( nullptr ),
m_pVertexBuffer( nullptr ),
m_AmbientColour(1.0)
{
	using namespace Genesis;

	ResourceManager* pResourceManager = FrameWork::GetResourceManager();

	m_pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/sectorbackground.glsl");

	ResourceImage* pBackground = (ResourceImage*)pResourceManager->GetResource("data/backgrounds/HeartOfFire.jpg");
	ShaderUniformSharedPtr pBackgroundSampler = m_pShader->RegisterUniform( "k_backgroundSampler", ShaderUniformType::Texture );
	pBackgroundSampler->Set( pBackground, GL_TEXTURE0 );

	m_pVertexBuffer = new VertexBuffer( GeometryType::Triangle, VBO_POSITION | VBO_UV );
	CreateGeometry();
}

Background::~Background()
{
	delete m_pVertexBuffer;
}

void Background::CreateGeometry()
{
	using namespace Genesis;

	ResourceManager* pResourceManager = FrameWork::GetResourceManager();
	ResourceImage* pBackground = (ResourceImage*)pResourceManager->GetResource("data/backgrounds/HeartOfFire.jpg");

	const glm::vec2 imageSize( static_cast<float>( pBackground->GetWidth() ), static_cast<float>( pBackground->GetHeight() ) );
	const glm::vec2 screenSize( static_cast<float>( Configuration::GetScreenWidth() ), static_cast<float>( Configuration::GetScreenHeight() ) );
	glm::vec2 uvScale = screenSize / imageSize;

	if ( uvScale.x > 1.0f )
	{
		uvScale.x = 1.0f;
		uvScale.y *= uvScale.x;
	}

	m_pVertexBuffer->CreateUntexturedQuad( 0.0f, 0.0f, screenSize.x, screenSize.y );

	const float uvs[] = {
        0.0f, uvScale.y, // 0
        0.0f, 0.0f, // 1
        uvScale.x, 0.0f, // 2
		0.0f, uvScale.y, // 0
		uvScale.x, 0.0f, // 2
        uvScale.x, uvScale.y // 3
    };

    m_pVertexBuffer->CopyData( uvs, 12, Genesis::VBO_UV );
}

void Background::Update( float delta )
{
    Genesis::SceneObject::Update( delta );
}

void Background::Render()
{
	m_pShader->Use();
	m_pVertexBuffer->Draw();
}

}