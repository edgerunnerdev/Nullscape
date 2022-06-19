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

#include "ui/debug/modelviewerobject.hpp"

#include "hexterminate.h"

#include <resources/resourcemodel.h>
#include <resourcemanager.h>
#include <genesis.h>
#include <math/misc.h>
#include <rendersystem.h>
#include <shader.h>
#include <shadercache.h>
#include <vertexbuffer.h>

namespace Hexterminate
{

ModelViewerObject::ModelViewerObject()
    : m_pShader(nullptr)
    , m_pVertexBufferX(nullptr)
    , m_pVertexBufferY(nullptr)
{
    using namespace Genesis;

    m_pModel = (Genesis::ResourceModel*)Genesis::FrameWork::GetResourceManager()->GetResource("data/models/ships/corvettes/dagger/exported/model.gmdl");

    m_pShader = FrameWork::GetRenderSystem()->GetShaderCache()->Load("untextured_vertex_coloured");
    m_pVertexBufferX = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_COLOUR);
    m_pVertexBufferX->CreateUntexturedQuad(0.0f, 0.0f, 50.0f, 5.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_pVertexBufferY = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_COLOUR);
    m_pVertexBufferY->CreateUntexturedQuad(0.0f, 0.0f, 5.0f, 50.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

ModelViewerObject::~ModelViewerObject()
{
}

void ModelViewerObject::Update(float delta)
{
    Genesis::SceneObject::Update(delta);
}

void ModelViewerObject::Render()
{
    m_pShader->Use();
    //m_pVertexBufferX->Draw();
    //m_pVertexBufferY->Draw();
    m_pModel->Render(glm::mat4());
}

} // namespace Hexterminate