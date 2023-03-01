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

#include <render/viewport.hpp>
#include <resources/resourceshader.hpp>
#include <scene/scenecamera.h>
#include <scene/sceneobject.h>
#include <vertexbuffer.h>

namespace Hyperscape
{

class SplitRenderer : public Genesis::SceneObject
{
public:
	SplitRenderer( const Genesis::ViewportSharedPtr& pViewport );
    virtual ~SplitRenderer();
    virtual void Update( float delta );
    virtual void Render( const Genesis::SceneCameraSharedPtr& pCamera );

private:
    Genesis::ViewportSharedPtr m_pViewport;
    Genesis::ResourceShader* m_pShader;
    Genesis::VertexBufferSharedPtr m_pVertexBuffer;
};

} // namespace Hyperscape
