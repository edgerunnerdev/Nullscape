// Copyright 2014 Pedro Nunes
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

#include "ui/debug/modelviewerbackground.hpp"

#include "hexterminate.h"

#include <genesis.h>
#include <math/misc.h>
#include <rendersystem.h>
#include <shader.h>
#include <shadercache.h>
#include <vertexbuffer.h>

namespace Hexterminate
{

ModelViewerBackground::ModelViewerBackground(int width, int height)
    : m_pShader(nullptr)
    , m_pVertexBuffer(nullptr)
{
    using namespace Genesis;

    m_pShader = FrameWork::GetRenderSystem()->GetShaderCache()->Load("untextured_vertex_coloured");
    m_pVertexBuffer = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_COLOUR);
    m_pVertexBuffer->CreateUntexturedQuad(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

ModelViewerBackground::~ModelViewerBackground()
{
    delete m_pVertexBuffer;
}

void ModelViewerBackground::Update(float delta)
{
    Genesis::SceneObject::Update(delta);
}

void ModelViewerBackground::Render()
{
    m_pShader->Use();
    m_pVertexBuffer->Draw();
}

} // namespace Hexterminate