// Copyright 2015 Pedro Nunes
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

#include <string>
#include <vector>
#include <scene/sceneobject.h>

namespace Genesis
{
	class ResourceImage;
	class ResourceShader;
	class VertexBuffer;
}

namespace Hyperscape
{

struct DustParticle
{
	float x, y, z;
};

typedef std::vector< DustParticle > DustVector;

class Dust: public Genesis::SceneObject
{
public:
	Dust();
	virtual ~Dust();
	virtual void Update(float fDelta) override;
	virtual void Render( const Genesis::SceneCameraSharedPtr& pCamera ) override;

private:
	Genesis::ResourceImage*		m_pDust;
	Genesis::ResourceShader*	m_pShader;
	Genesis::VertexBuffer*		m_pVertexBuffer;
	DustVector					m_dustParticles;
};

}
