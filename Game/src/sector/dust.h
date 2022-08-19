// Copyright 2015 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

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

namespace Nullscape
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
	virtual void Update(float fDelta);
	virtual void Render();

private:
	Genesis::ResourceImage*		m_pDust;
	Genesis::ResourceShader*	m_pShader;
	Genesis::VertexBuffer*		m_pVertexBuffer;
	DustVector					m_dustParticles;
};

}
