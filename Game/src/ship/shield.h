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

#include <scene/sceneobject.h>
#include <resources/resourceshader.hpp>
#include <rendersystem.h>
#include <vertexbuffer.h>

#include "ship/shipcollisioninfo.h"
#include "ship/shieldhitregistry.h"
#include "ship/moduleinfo.h"
#include "ship/module.h"

namespace Genesis
{
	class ResourceImage;
	class ResourceShader;

	namespace Physics
	{
		class Shape;
		class Ghost;
		GENESIS_DECLARE_SMART_PTR( Ghost );
	};
};

namespace Hyperscape
{

class Ship;
class ShipCollisionInfo;

static const Uint32 sMaxShieldPoints = 180;

enum class ShieldState
{
	Deactivated,
	Activating,
	Activated,
	Deactivating
};

class Shield : public Genesis::SceneObject
{
public:
									Shield( Ship* pShip );
	virtual							~Shield();

	virtual void					Update( float delta ) override;
    virtual void                    Render() override {};
	virtual void					Render( const glm::mat4& modelTransform );

	void							InitialisePhysics( const glm::vec3& translation, float radiusX, float radiusY );

	float							GetCurrentHealthPoints() const;
	float							GetMaximumHealthPoints() const;
	float							GetRechargeRate() const;
	ShieldState						GetQuantumState() const;

	void							ApplyDamage( float displayAmount, float frameAmount, float angle, WeaponSystem weaponSystem, DamageType damageType, Ship* pDealtBy );
	void							Deactivate();

	static float					CalculateEfficiency( const ShieldModuleList& shieldModules );

private:
	void							CreateGeometry();
	void							UpdateColour();
	void							RenderRegularShield( const glm::mat4& modelTransform );
	void							RenderQuantumShield( const glm::mat4& modelTransform );

	Ship*							m_pOwner;
		
	Genesis::ResourceImage*			m_pTexture;
	Genesis::ResourceShader*		m_pShader;
	Genesis::VertexBuffer*          m_pVertexBuffer;
    Genesis::ColourData             m_ColourData;
	Genesis::ShaderUniformSharedPtr	m_pQuantumModeUniform;
    Genesis::ShaderUniformSharedPtr m_pShieldStrengthUniform;
	Genesis::ShaderUniformSharedPtr m_pQuantumShieldScaleUniform;
	Genesis::ShaderUniformSharedPtr m_pQuantumShieldTriangleGapUniform;
	Genesis::ShaderUniformSharedPtr m_pQuantumShieldIntensityUniform;
	Genesis::ShaderUniformSharedPtr	m_pQuantumShieldColourUniform;
    bool                            m_VisualsDirty;

	int								m_ShapeKey;
	float							m_RadiusX;
	float							m_RadiusY;

	float							m_RechargeRate;			// Maximum hit points per second that the shield is capable of regenerating
	float							m_MaximumHitPoints;
	float							m_CurrentHitPoints;
	float							m_DeactivatedTimer;		// Time, in seconds, until the shield is reactivated after it is taken down
	float							m_ActivationRatio;		// For rendering purposes. 0.0f: disabled, 1.0f: enabled
	float							m_EmergencyCapacitorsCooldown;
	ShieldState						m_State;

	ShieldHitRegistry				m_HitRegistry;

	Genesis::Physics::GhostUniquePtr m_pGhost;
	ShipCollisionInfoUniquePtr		m_pCollisionInfo;
};

inline float Shield::GetCurrentHealthPoints() const
{
	return m_CurrentHitPoints;
}

inline float Shield::GetMaximumHealthPoints() const
{
	return m_MaximumHitPoints;
}

inline float Shield::GetRechargeRate() const
{
	return m_RechargeRate;
}

inline ShieldState Shield::GetQuantumState() const
{
	return m_State;
}

}