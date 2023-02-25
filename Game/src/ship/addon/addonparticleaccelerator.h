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
#include <random>

#include "laser/laser.h"
#include "ship/addon/addon.h"

namespace Genesis
{
class ResourceSound;
}

namespace Hyperscape
{

static const unsigned int sParticleAcceleratorLaserCount = 24;
static const float sParticleAcceleratorLaserDuration = 1.0f; // in seconds

class AddonParticleAccelerator : public Addon
{
public:
					AddonParticleAccelerator( AddonModule* pModule, Ship* pOwner );
	virtual			~AddonParticleAccelerator() {};

	virtual void	Activate() override;
	virtual void	Update( float delta ) override;
	virtual bool	CanUse() const override;

private:
	void			PlaySFX();
	void			UpdateFiring( float delta );
	void			UpdateLaser( float delta );
	float			ProcessCollision( bool appliesDamage, float delta );
	void			CalculateVectors( glm::vec3& source, glm::vec3& target, glm::vec3& right ) const;
	
	enum class Stage
	{
		Aiming,
		Firing,
		Done
	};

	struct ParticleLaser
	{
		float m_Timer;
		float m_Duration;
		float m_Width;
		float m_Offset;
		Laser m_Laser;
		Genesis::Colour m_Colour;
	};

	std::array< ParticleLaser, sParticleAcceleratorLaserCount > m_ParticleAcceleratorLasers;
	ParticleLaser m_AimingLaser;

	Genesis::ResourceSound* m_pSFX;
	int m_TimesToFire;
	float m_TimeToNextShot;
	Stage m_Stage;
	std::default_random_engine m_RandomEngine;
	glm::vec3 m_EmitterOffset;
};

}
