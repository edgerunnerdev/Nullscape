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

Viewport::Viewport(const std::string& name, int width, int height, bool hasDepth, bool hasStencil, SceneSharedPtr pScene /* = nullptr */)
    : m_Width(width)
    , m_Height(height)
    , m_Name(name)
{
    SDL_assert(width > 0);
    SDL_assert(height > 0);
    m_Width = width;
    m_Height = height;
    m_pRenderTarget = std::move(FrameWork::GetRenderSystem()->CreateRenderTarget(name, width, height, hasDepth, hasStencil, true));

    if (pScene == nullptr)
    {
        m_pScene = std::make_shared<Genesis::Scene>();
    }
    else
    {
        m_pScene = pScene;
    }
}

Viewport::~Viewport() {}

void Viewport::Render()
{
    RenderSystem* pRenderSystem = FrameWork::GetRenderSystem();
    pRenderSystem->SetRenderTarget(m_pRenderTarget);
    m_pRenderTarget->Clear();
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    m_pScene->Render(this);
    glDisable(GL_DEPTH_TEST);
    pRenderSystem->SetRenderTarget(pRenderSystem->GetPrimaryViewport()->GetRenderTarget());
}

RenderTargetSharedPtr Viewport::GetRenderTarget()
{
    return m_pRenderTarget;
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
