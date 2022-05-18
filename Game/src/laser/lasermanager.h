// Copyright 2015 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <vector>
#include <genesis.h>
#include <rendersystem.h>
#include <resourcemanager.h>
#include <scene/sceneobject.h>
#include "laser/laser.h"

namespace Genesis
{
class ResourceImage;
class VertexBuffer;
}


namespace Hexterminate
{

typedef std::vector< Laser > LaserVector;


///////////////////////////////////////////////////////////////////////////////
// LaserManager
// Draws all the lasers as a single draw call
///////////////////////////////////////////////////////////////////////////////

static const unsigned int sLaserManagerCapacity = 512;

class LaserManager: public Genesis::SceneObject
{
public:
									LaserManager();
	virtual							~LaserManager() override;

	virtual void					Update( float delta ) override;
	virtual void					Render() override;

	void							AddLaser( const Laser& laser );

private:
	LaserVector						m_Lasers;

	Genesis::ResourceImage*			m_pTexture;
	Genesis::Shader*				m_pShader;
	Genesis::VertexBuffer*			m_pVertexBuffer;
};

}