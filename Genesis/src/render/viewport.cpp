// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include "render/viewport.hpp"

#include "genesis.h"
#include "render/rendertarget.h"
#include "rendersystem.h"
#include "scene/scene.h"

#include <SDL.h>

namespace Genesis
{

Viewport::Viewport( const std::string& name, int width, int height, SceneSharedPtr pScene, SceneCameraSharedPtr pCamera, bool hasDepth /* = true */, bool hasStencil /* = true */ )
    : m_pScene( pScene )
    , m_pCamera( pCamera )
    , m_Width( width )
    , m_Height( height )
    , m_Name( name )
{
    SDL_assert( width > 0 );
    SDL_assert( height > 0 );
    SDL_assert( pScene );
    SDL_assert( pCamera );

    m_Width = width;
    m_Height = height;
    m_pRenderTarget = FrameWork::GetRenderSystem()->CreateRenderTarget( name, width, height, hasDepth, hasStencil, true );
}

Viewport::~Viewport() {}

void Viewport::Render()
{
    RenderSystem* pRenderSystem = FrameWork::GetRenderSystem();
    pRenderSystem->SetRenderTarget( m_pRenderTarget );
    m_pRenderTarget->Clear();
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    m_pScene->Render( this );
    glDisable( GL_DEPTH_TEST );
    pRenderSystem->SetRenderTarget( pRenderSystem->GetPrimaryViewport()->GetRenderTarget() );
}

} // namespace Genesis
