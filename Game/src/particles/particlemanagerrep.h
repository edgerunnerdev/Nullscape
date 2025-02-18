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

#include <scene/sceneobject.h>
#include <rendersystem.h>
#include "particles/particlepass.h"

namespace Genesis::Gui
{
class Atlas;
}

namespace Hyperscape
{

class ParticleManager;
class ParticlePass;
class Particle;

static const int sNumParticlePasses = 2;

///////////////////////////////////////////////////////////////////////////////
// ParticleManagerRep
///////////////////////////////////////////////////////////////////////////////

class ParticleManagerRep : public Genesis::SceneObject
{
public:
								ParticleManagerRep( ParticleManager* pParticleManager);
	virtual						~ParticleManagerRep();
	void						SetParticleManager( ParticleManager* pParticleManager );
	void						Update( float delta ) override;
	void						Render( const Genesis::SceneCameraSharedPtr& pCamera ) override;

private:
	int							FindIndexForTexture( ParticlePass* pPass, int id );
	void						AddQuad( const Genesis::Gui::Atlas* pAtlas, const Particle* pParticle, Genesis::PositionData& vertices, Genesis::UVData& uvs, Genesis::ColourData& colours );
	void						RenderGeometry( ParticlePass* pPass, const ParticleRenderData& particleRenderData, unsigned int startIdx, unsigned int endIdx );
	Genesis::ResourceShader*	GetShader( Genesis::BlendMode blendMode, int textureId );
	ParticleManager*			m_pParticleManager;
	ParticlePass*				m_pPass[2];
};

inline void ParticleManagerRep::SetParticleManager( ParticleManager* pParticleManager )
{
	m_pParticleManager = pParticleManager;
}

}
