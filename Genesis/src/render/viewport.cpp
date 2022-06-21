// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
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

#include "render/viewport.hpp"

#include "genesis.h"
#include "render/rendertarget.h"
#include "rendersystem.h"
#include "scene/scene.h"

#include <SDL.h>

namespace Genesis
{

Viewport::Viewport(const std::string& name, int width, int height, bool hasDepth, bool hasStencil)
    : m_Width(width)
    , m_Height(height)
    , m_Name(name)
{
    SDL_assert(width > 0);
    SDL_assert(height > 0);
    m_Width = width;
    m_Height = height;
    m_pRenderTarget = RenderTarget::Create(name, width, height, hasDepth, hasStencil);
    m_pScene = std::make_shared<Genesis::Scene>();
}

Viewport::~Viewport() {}

void Viewport::Render()
{
    RenderSystem* pRenderSystem = FrameWork::GetRenderSystem();
    pRenderSystem->SetRenderTarget(m_pRenderTarget.get());
    m_pRenderTarget->Clear();
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    m_pScene->Render(this);
    glDisable(GL_DEPTH_TEST);
    pRenderSystem->SetRenderTarget(RenderTargetId::Default);
}

RenderTarget* Viewport::GetRenderTarget()
{
    return m_pRenderTarget.get();
}

Scene* Viewport::GetScene()
{
    return m_pScene.get();
}
int Viewport::GetWidth()
{
    return m_Width;
}
int Viewport::GetHeight()
{
    return m_Height;
}

const std::string& Viewport::GetName() const 
{
    return m_Name;
}

} // namespace Genesis
