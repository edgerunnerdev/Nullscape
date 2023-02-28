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

#include <vector>
#include <genesis.h>
#include <rendersystem.h>
#include <resourcemanager.h>
#include <scene/sceneobject.h>
#include "sprite/sprite.h"

namespace Genesis
{
class ResourceImage;
class ResourceShader;
class VertexBuffer;
}


namespace Hyperscape
{

typedef std::vector< Sprite > SpriteVector;


///////////////////////////////////////////////////////////////////////////////
// SpriteManager
// Draws all the sprites as a single draw call
///////////////////////////////////////////////////////////////////////////////

class SpriteManager: public Genesis::SceneObject
{
public:
									SpriteManager();
	virtual							~SpriteManager() override;

	virtual void					Update( float delta ) override;
	virtual void					Render( const Genesis::SceneCameraSharedPtr& pCamera ) override;

	void							AddSprite( const Sprite& Sprite );

private:
	SpriteVector					m_Sprites;

	Genesis::ResourceImage*			m_pTexture;
	Genesis::ResourceShader*		m_pShader;
	Genesis::VertexBuffer*			m_pVertexBuffer;
};

}