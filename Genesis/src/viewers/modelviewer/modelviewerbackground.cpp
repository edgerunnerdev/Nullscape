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

#include "viewers/modelviewer/modelviewerbackground.hpp"

#include "genesis.h"
#include "math/misc.h"
#include "rendersystem.h"
#include "vertexbuffer.h"

namespace Genesis
{

ModelViewerBackground::ModelViewerBackground( int width, int height )
    : m_pShader( nullptr )
    , m_pVertexBuffer( nullptr )
{
    m_pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>( "data/shaders/untextured_vertex_coloured.glsl" );
    m_pVertexBuffer = new VertexBuffer( GeometryType::Triangle, VBO_POSITION | VBO_COLOUR );
    m_pVertexBuffer->CreateUntexturedQuad( 0.0f, 0.0f, static_cast<float>( width ), static_cast<float>( height ), glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

ModelViewerBackground::~ModelViewerBackground()
{
    delete m_pVertexBuffer;
}

void ModelViewerBackground::Update( float delta )
{
    Genesis::SceneObject::Update( delta );
}

void ModelViewerBackground::Render( const SceneCameraSharedPtr& pCamera )
{
    m_pShader->Use();
    m_pVertexBuffer->Draw();
}

} // namespace Genesis
