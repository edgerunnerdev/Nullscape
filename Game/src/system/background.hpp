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
    virtual void Render( const Genesis::SceneCameraSharedPtr& pCamera ) override;

    const glm::vec4& GetAmbientColour() const;

private:
    enum class Anchor
    {
        BottomLeft,
        TopLeft,
        TopRight,
        BottomRight
    };

    void CreateGeometry();
    void SetupCameraAnchors();
    void CalculateCameraOffsets();
    glm::mat4x4 AnchorToOffset( const Anchor& anchor ) const;

    Genesis::ResourceShader* m_pShader;
    Genesis::VertexBufferUniquePtr m_pVertexBuffer;
    glm::vec4 m_AmbientColour;
    float m_Size;
    Anchor m_PrimaryCameraAnchor;
    Anchor m_SecondaryCameraAnchor;
    glm::mat4x4 m_PrimaryCameraOffset;
    glm::mat4x4 m_SecondaryCameraOffset;
};

inline const glm::vec4& Background::GetAmbientColour() const
{
    return m_AmbientColour;
}

} // namespace Hyperscape