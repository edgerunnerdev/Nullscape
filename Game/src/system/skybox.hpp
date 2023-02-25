// Copyright 2022 Pedro Nunes
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

#include <rendersystem.fwd.h>
#include <render/rendertarget.h>
#include <resources/resourceshader.hpp>
#include <scene/sceneobject.h>

#include <string>

namespace Genesis
{
class VertexBuffer;
}

namespace Hyperscape
{

class Skybox : public Genesis::SceneObject
{
public:
    Skybox(const std::string& seed);
    virtual ~Skybox() override;
    virtual void Update(float delta) override;
    virtual void Render() override;

    const glm::vec4& GetAmbientColour() const;

private:
    void CreateCubemapTexture();
    void CreateGeometry();
    void RenderProteanClouds();

    Genesis::ResourceShader* m_pProteanCloudsShader;
    Genesis::ResourceShader* m_pSkyboxShader;
    Genesis::VertexBuffer* m_pVertexBuffer;
    glm::vec4 m_AmbientColour;
    bool m_ProteanCloudsGenerated;
    GLuint m_Cubemap;
    int m_Resolution;
};

inline const glm::vec4& Skybox::GetAmbientColour() const
{
    return m_AmbientColour;
}

} // namespace Hyperscape