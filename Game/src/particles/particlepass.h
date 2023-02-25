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

#include <vector>

#include <resources/resourceshader.hpp>
#include <rendersystem.h>
#include <vertexbuffer.h>

#include "particles/particleemitter.h"

namespace Genesis
{
	namespace Gui
	{
		class Atlas;
	}
}

namespace Hyperscape
{

class Particle;
class ParticleManager;
class ParticleEmitter;

typedef std::vector< const Particle* > ParticlePointerVector;

struct ParticleRenderData
{
	int textureId;
	const Genesis::Gui::Atlas* pAtlas;
	ParticlePointerVector particles;
};

typedef std::vector< ParticleRenderData > EmitterRenderData;

class ParticlePass
{
public:
								ParticlePass( Genesis::BlendMode blendMode, const std::string& shader, bool glowEnabled );
								~ParticlePass();
	
	bool						m_GlowEnabled;

	Genesis::BlendMode			m_BlendMode;
	Genesis::ResourceShader*	m_pShader;
	Genesis::ShaderUniformSharedPtr m_pSamplerUniform;

	Genesis::VertexBuffer*		m_pVertexBuffer;
	Genesis::PositionData		m_PositionData;
	Genesis::UVData				m_UVData;
	Genesis::ColourData			m_ColourData;

	EmitterRenderData			m_Data;
};

}
