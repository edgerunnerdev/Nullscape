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

#include "ammo/ammo.h"

namespace Genesis
{
	class ResourceImage;
	class ResourceShader;
    class VertexBuffer;
}

namespace Hyperscape
{

class Weapon;


///////////////////////////////////////////////////////////////////////////////
// Beam
///////////////////////////////////////////////////////////////////////////////

class Beam: public Ammo
{
public:
										Beam();
	virtual								~Beam();

	virtual void						Create( Weapon* pWeapon, float additionalRotation = 0.0f  ) override;
	virtual void						Update( float delta ) override;
	virtual void						Render() override;

protected:
	virtual float						GetOpacity() const;

private:
	void								SetupBeam();
	void								SetupBeamFlare();
	void								RenderBeam( const glm::mat4& modelMatrix, const Genesis::Colour& beamColour, float opacity );
	void								RenderBeamFlare( const glm::mat4& modelMatrix, const Genesis::Colour& beamColour, float opacity );

	static Genesis::ResourceShader*		m_pShader;
	static Genesis::ResourceShader*		m_pFlareShader;

    Genesis::VertexBuffer*              m_pBeamVertexBuffer;
    Genesis::VertexBuffer*              m_pFlareVertexBuffer;
};

inline float Beam::GetOpacity() const
{
	return 1.0f;
}

}