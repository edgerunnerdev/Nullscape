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

#include <render/rendertarget.h>
#include <resources/resourceshader.hpp>
#include <scene/sceneobject.h>
#include <vertexbuffer.h>

#include <string>

namespace Genesis
{
class VertexBuffer;
}

namespace Hyperscape
{

class Background : public Genesis::SceneObject
{
public:
    Background(const std::string& seed);
    virtual ~Background() override;
    virtual void Update(float delta) override;
    virtual void Render() override;

    const glm::vec4& GetAmbientColour() const;

private:
    void CreateGeometry();

    Genesis::ResourceShader* m_pShader;
    Genesis::VertexBufferUniquePtr m_pVertexBuffer;
    glm::vec4 m_AmbientColour;
    float m_Size;
};

inline const glm::vec4& Background::GetAmbientColour() const
{
    return m_AmbientColour;
}

} // namespace Hyperscape