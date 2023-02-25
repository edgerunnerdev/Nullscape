// Copyright 2016 Pedro Nunes
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

#include <array>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <resources/resourceshader.hpp>
#include "rendersystem.fwd.h"

namespace Hyperscape
{

enum class ShipShaderUniform
{
	PrimaryPaint = 0,
	SecondaryPaint,
	Health,
	RepairEdgeAlpha,
	RepairEdgeOffset,
	ClipActive,
	Clip,
	ClipForward,
	AmbientColour,
	EmissiveColour,
	OverlayColour,
	EMPActive,
    DiffuseMap,
    SpecularMap,
    PaintMap,
    DamageMap,
    Count
};

class ShipShaderUniforms
{
public:
	ShipShaderUniforms();
	
    Genesis::ResourceShader* GetShader() const;

    void Set( ShipShaderUniform shipShaderUniform, int value );
    void Set( ShipShaderUniform shipShaderUniform, float value );
    void Set( ShipShaderUniform shipShaderUniform, const glm::vec4& value );
    void Set( ShipShaderUniform shipShaderUniform, Genesis::ResourceImage* pTexture );

	Genesis::ShaderUniformSharedPtr Get( ShipShaderUniform shipShaderUniform ) const;

private:
    GLenum UniformToGL( ShipShaderUniform uniform ) const;

    typedef std::array< Genesis::ShaderUniformSharedPtr, static_cast< size_t >( ShipShaderUniform::Count ) > ShaderUniformArray;
    ShaderUniformArray m_Uniforms;
    Genesis::ResourceShader* m_pShader;
};

inline Genesis::ResourceShader* ShipShaderUniforms::GetShader() const
{
    return m_pShader;
}

inline Genesis::ShaderUniformSharedPtr ShipShaderUniforms::Get( ShipShaderUniform shipShaderUniform ) const
{
	return m_Uniforms[ static_cast< size_t >( shipShaderUniform ) ];
}

}
