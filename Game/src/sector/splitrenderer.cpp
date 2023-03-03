// Copyright 2023 Pedro Nunes
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

#include "sector/splitrenderer.hpp"

#include <configuration.h>
#include <genesis.h>
#include <rendersystem.h>
#include <resourcemanager.h>
#include <shaderuniform.h>

namespace Hyperscape
{

SplitRenderer::SplitRenderer( const Genesis::ViewportSharedPtr& pViewport )
    : m_pViewport( pViewport )
    , m_pShader( nullptr )
{
    using namespace Genesis;

    m_pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>( "data/shaders/split.glsl" );
    ShaderUniformSharedPtr pSampler = m_pShader->RegisterUniform( "k_sampler0", ShaderUniformType::Texture );
    pSampler->Set( pViewport->GetRenderTarget()->GetColor(), GL_TEXTURE0 );
    m_pVertexBuffer = std::make_shared<VertexBuffer>( GeometryType::Triangle, VBO_POSITION | VBO_UV );
    m_pVertexBuffer->CreateUntexturedQuad( 0.0f, 0.0f, static_cast<float>( Configuration::GetScreenWidth() ), static_cast<float>( Configuration::GetScreenHeight() ) );
    const float uvs[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f }; // Flipping V. This shouldn't be needed, something wrong with the post chain?
    m_pVertexBuffer->CopyData( uvs, 12, Genesis::VBO_UV );
}

SplitRenderer::~SplitRenderer()
{
}

void SplitRenderer::Update( float delta )
{
}

void SplitRenderer::Render( const Genesis::SceneCameraSharedPtr& pCamera )
{
    using namespace Genesis;
    FrameWork::GetRenderSystem()->SetBlendMode( Genesis::BlendMode::Blend );
    m_pShader->Use();
    m_pVertexBuffer->Draw();
    FrameWork::GetRenderSystem()->SetBlendMode( Genesis::BlendMode::Disabled );
}

} // namespace Hyperscape
