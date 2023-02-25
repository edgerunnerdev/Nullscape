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

class Boundary: public Genesis::SceneObject
{
public:
								Boundary();
	virtual						~Boundary();
	virtual void				Update( float delta ) override;
	virtual void				Render() override;


private:
	Genesis::ResourceShader*	m_pShader;
	Genesis::VertexBuffer*		m_pVertexBuffer;
	float						m_DestructionTimer;
	float						m_WarningTimer;
};

}
