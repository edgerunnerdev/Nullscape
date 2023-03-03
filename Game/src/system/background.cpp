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

#include "system/background.hpp"

#include "game.hpp"
#include "misc/random.h"
#include "player.h"

#include <algorithm>
#include <configuration.h>
#include <genesis.h>
#include <math/misc.h>
#include <render/viewport.hpp>
#include <rendersystem.h>
#include <resources/resourceimage.h>
#include <resources/resourcemodel.h>
#include <shaderuniform.h>
#include <sstream>
#include <vertexbuffer.h>

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// Background
///////////////////////////////////////////////////////////////////////////////

Background::Background( const std::string& seed )
    : m_pShader( nullptr )
    , m_AmbientColour( 1.0 )
    , m_Size( 512.0f )
    , m_PrimaryCameraAnchor( Anchor::BottomLeft )
    , m_SecondaryCameraAnchor( Anchor::BottomLeft )
    , m_PrimaryCameraOffset( 0.0f )
    , m_SecondaryCameraOffset( 0.0f )
{
    using namespace Genesis;

    ResourceManager* pResourceManager = FrameWork::GetResourceManager();
    m_pShader = pResourceManager->GetResource<ResourceShader*>( "data/shaders/sectorbackground.glsl" );
    ResourceImage* pBackground = pResourceManager->GetResource<ResourceImage*>( "data/backgrounds/background1.jpg" );
    const glm::vec2 screenSize( static_cast<float>( Configuration::GetScreenWidth() ), static_cast<float>( Configuration::GetScreenHeight() ) );
    const glm::vec2 imageSize( static_cast<float>( pBackground->GetWidth() ), static_cast<float>( pBackground->GetHeight() ) );
    m_Size = glm::max( glm::max( screenSize.x, screenSize.y ), glm::max( imageSize.x, imageSize.y ) );

    ShaderUniformSharedPtr pBackgroundSampler = m_pShader->RegisterUniform( "k_backgroundSampler", ShaderUniformType::Texture );
    pBackgroundSampler->Set( pBackground, GL_TEXTURE0 );

    CreateGeometry();
    SetupCameraAnchors();
    CalculateCameraOffsets();
}

Background::~Background()
{
}

void Background::CreateGeometry()
{
    using namespace Genesis;
    m_pVertexBuffer = std::make_unique<VertexBuffer>( GeometryType::Triangle, VBO_POSITION | VBO_UV );
    m_pVertexBuffer->CreateTexturedQuad( 0.0f, 0.0f, m_Size, m_Size );
}

void Background::SetupCameraAnchors()
{
    // The primary anchor is always bottom left or top right, while the secondary anchor is diagonally opposite.
    m_PrimaryCameraAnchor = ( Random::Next() % 2 == 0 ) ? Anchor::BottomLeft : Anchor::TopLeft;
    m_SecondaryCameraAnchor = ( m_PrimaryCameraAnchor == Anchor::BottomLeft ) ? Anchor::TopRight : Anchor::BottomRight;
}

void Background::CalculateCameraOffsets()
{
    m_PrimaryCameraOffset = AnchorToOffset( m_PrimaryCameraAnchor );
    m_SecondaryCameraOffset = AnchorToOffset( m_SecondaryCameraAnchor );
}

glm::mat4x4 Background::AnchorToOffset( const Anchor& anchor ) const
{
    using namespace Genesis;
    const glm::vec2 screenSize( static_cast<float>( Configuration::GetScreenWidth() ), static_cast<float>( Configuration::GetScreenHeight() ) );

    glm::vec2 offset;
    if ( anchor == Anchor::BottomLeft )
    {
        offset = glm::vec2( 0.0f, 0.0f );
    }
    else if ( anchor == Anchor::BottomRight )
    {
        offset = glm::vec2( -m_Size + screenSize.x, 0.0f );
    }
    else if ( anchor == Anchor::TopLeft )
    {
        offset = glm::vec2( 0.0f, -m_Size + screenSize.y );
    }
    else if ( anchor == Anchor::TopRight )
    {
        offset = glm::vec2( -m_Size + screenSize.x, -m_Size + screenSize.y );
    }

    return glm::translate( glm::vec3( offset.x, offset.y, 0.0f ) );
}

void Background::Update( float delta )
{
    Genesis::SceneObject::Update( delta );
}

void Background::Render( const Genesis::SceneCameraSharedPtr& pCamera )
{
    const bool isPrimaryCamera = ( Genesis::FrameWork::GetScene()->GetCamera() == pCamera );
    const glm::mat4x4& cameraOffset = isPrimaryCamera ? m_PrimaryCameraOffset : m_SecondaryCameraOffset;

    m_pShader->Use( cameraOffset );
    m_pVertexBuffer->Draw();
}

} // namespace Hyperscape