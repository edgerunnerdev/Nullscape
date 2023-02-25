// Copyright 2014 Pedro Nunes
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

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec4.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <rendersystem.h>
#include <scene/sceneobject.h>
#include <vertexbuffer.h>

namespace Genesis
{
class ResourceImage;
class ResourceShader;
} // namespace Genesis

namespace Hyperscape
{

class TrailManager;
class Trail;

class TrailManagerRep : public Genesis::SceneObject
{
public:
    TrailManagerRep(TrailManager* pTrailManager);
    virtual ~TrailManagerRep() override;
    virtual void Update(float delta) override;
    virtual void Render() override;

private:
    void PushBackUVs(Genesis::UVData& uvData);
    void PushBackColours(Genesis::ColourData& colourData, const glm::vec4& color, float initialWidth, float currentWidth, float nextWidth);

    TrailManager* m_pTrailManager;
    Genesis::ResourceShader* m_pShader;
    Genesis::VertexBuffer* m_pVertexBuffer;
    unsigned int m_NumVertices;
};

} // namespace Hyperscape