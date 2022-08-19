// Copyright 2022 Pedro Nunes
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

#include "system/background.hpp"

#include "hexterminate.h"
#include "player.h"

#include <algorithm>
#include <configuration.h>
#include <genesis.h>
#include <math/misc.h>
#include <rendersystem.h>
#include <render/viewport.hpp>
#include <resources/resourceimage.h>
#include <resources/resourcemodel.h>
#include <shaderuniform.h>
#include <sstream>
#include <vertexbuffer.h>

namespace Nullscape
{

///////////////////////////////////////////////////////////////////////////////
// Background
///////////////////////////////////////////////////////////////////////////////

Background::Background(const std::string& seed)
    : m_pProteanCloudsShader(nullptr)
    , m_pShader(nullptr)
    , m_pVertexBuffer(nullptr)
    , m_AmbientColour(1.0)
    , m_ProteanCloudsGenerated(false)
{
    using namespace Genesis;

    ResourceManager* pResourceManager = FrameWork::GetResourceManager();

    m_pProteanCloudsShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/proteanclouds.glsl");
    m_pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>("data/shaders/sectorbackground.glsl");

    //ResourceImage* pBackground = (ResourceImage*)pResourceManager->GetResource("data/backgrounds/HeartOfFire.jpg");
    //ShaderUniformSharedPtr pBackgroundSampler = m_pShader->RegisterUniform("k_backgroundSampler", ShaderUniformType::Texture);
    //pBackgroundSampler->Set(pBackground, GL_TEXTURE0);

    m_pVertexBuffer = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_UV);
    CreateGeometry();
}

Background::~Background()
{
    delete m_pVertexBuffer;
}

void Background::CreateGeometry()
{
    using namespace Genesis;

    const glm::vec2 screenSize(static_cast<float>(Configuration::GetScreenWidth()), static_cast<float>(Configuration::GetScreenHeight()));
    m_pVertexBuffer->CreateTexturedQuad(0.0f, 0.0f, screenSize.x, screenSize.y);
}

void Background::Update(float delta)
{
    Genesis::SceneObject::Update(delta);
}

void Background::Render()
{
    using namespace Genesis;

    if (m_ProteanCloudsGenerated == false)
    {
        RenderSystem* pRenderSystem = FrameWork::GetRenderSystem();
        Viewport* pPrimaryViewport = pRenderSystem->GetPrimaryViewport();
        m_pProteanCloudsRenderTarget = FrameWork::GetRenderSystem()->CreateRenderTarget("Protean clouds", pPrimaryViewport->GetWidth(), pPrimaryViewport->GetHeight(), false, false, false);
        pRenderSystem->SetRenderTarget(m_pProteanCloudsRenderTarget);
        m_pProteanCloudsShader->Use();
        m_pVertexBuffer->Draw();
        pRenderSystem->SetRenderTarget(pPrimaryViewport->GetRenderTarget());
        m_ProteanCloudsGenerated = true;

        ShaderUniformSharedPtr pBackgroundSampler = m_pShader->RegisterUniform("k_backgroundSampler", ShaderUniformType::Texture);
        pBackgroundSampler->Set(m_pProteanCloudsRenderTarget->GetColor(), GL_TEXTURE0);
    }

    m_pShader->Use();
    m_pVertexBuffer->Draw();
}

} // namespace Nullscape