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

#include <cassert>

#include <math/constants.h>
#include <resources/resourcemodel.h>

#include "ammo/missile.h"
#include "sector/sector.h"
#include "sprite/spritemanager.h"
#include "sprite/sprite.h"
#include "trail/trailmanager.h"
#include "trail/trail.h"
#include "ship/ship.h"
#include "ship/weapon.h"
#include "game.hpp"

namespace Hyperscape
{

Missile::Missile():
m_pModel( nullptr ),
m_pTargetShip( nullptr ),
m_LaunchTimer( 0.0f ),
m_GlowSize( 30.0f ),
m_GlowColour( 0.8f, 0.8f, 0.8f, 1.0f )
{
	m_SwarmTimer = (float)(rand() % 314) / 100.0f;
}

Missile::~Missile()
{
    TrailSharedPtr pTrail = m_pTrail.lock();
    if (pTrail)
    {
        pTrail->SetOrphan();
	}
}

MissileType Missile::GetType() const
{
	return MissileType::Missile;
}

const std::string Missile::GetResourceName() const
{
	static const std::string sResourceName( "data/models/ammo/missile.tmf" );
	return sResourceName;
}

void Missile::Create( Weapon* pWeapon, float additionalRotation /* = 0.0f */ )
{
	Ammo::Create( pWeapon, additionalRotation );

	m_pModel = (Genesis::ResourceModel*)Genesis::FrameWork::GetResourceManager()->GetResource( GetResourceName() );
	m_pTargetShip = FindClosestShip( pWeapon->GetTargetPosition() );
    CreateTrail();
}

void Missile::GetTrailProperties(float& initialWidth, float& decay, glm::vec4& color) const 
{
    initialWidth = 2.0f;
    decay = 2.0f;
	color = glm::vec4( 0.5f, 0.5f, 0.5f, 0.5f );
}

void Missile::CreateTrail() 
{
 //   float initialWidth;
 //   float decay;
 //   glm::vec4 color;
 //   GetTrailProperties(initialWidth, decay, color);
 //   m_pTrail = g_pGame->GetCurrentSector()->GetTrailManager()->Add(initialWidth, decay, color);
 //   TrailSharedPtr pTrail = m_pTrail.lock();
 //   if (pTrail)
 //   {
 //       pTrail->AddPoint(m_Src);
	//}
}

Ship* Missile::FindClosestShip( const glm::vec3& position )
{
	const ShipList& ships = g_pGame->GetCurrentSector()->GetShipList();
	float closestDistance = FLT_MAX;
	Ship* pClosestTarget = nullptr;
	for ( auto& pShip : ships )
	{
		// Don't target friendly ships
		if ( Faction::sIsEnemyOf( pShip->GetFaction(), m_pOwner->GetOwner()->GetFaction() ) == false )
			continue;

		// Or dead ships...
		if ( pShip->IsTerminating() || pShip->IsDestroyed() )
			continue;

		// Or ships that are having their bridge removed (because they are in edit mode!)
		if ( pShip->GetTowerModule() == nullptr )
			continue;

		float distance = glm::distance( pShip->GetTowerPosition(), position );
		if ( distance < closestDistance )
		{
			closestDistance = distance;
			pClosestTarget = pShip;
		}
	}
	return pClosestTarget;
}

void Missile::Update( float delta )
{
	if ( m_Range <= 0.0f )
	{
		Kill();
		m_pOwner->ResetSiegebreakerStacks();
	}
	else
	{
		if ( m_pOwner->GetInfo()->GetTracking() > 0.0f )
		{
			UpdateTarget( delta );
			TrackTarget( delta );
		}

		m_Src += m_Dir * m_Speed * delta;
		m_Dst += m_Dir * m_Speed * delta;

		TrailSharedPtr pTrail = m_pTrail.lock();
        if (pTrail)
        {
            pTrail->AddPoint(m_Dst);
		}

		m_Range -= m_Speed * delta;

		m_LaunchTimer += delta;
		m_SwarmTimer += delta;

		UpdateGlow();
	}
}

void Missile::UpdateTarget( float delta )
{
	// Stick to a target unless it has died
	if ( m_pTargetShip != nullptr && !m_pTargetShip->IsDestroyed() )
		return;

	// Attempt to find the closest ship to the missile
	m_pTargetShip = FindClosestShip( m_Src );
}

void Missile::TrackTarget( float delta )
{
	if ( m_pTargetShip == nullptr )
		return;

	glm::vec3 wantedDir = m_pTargetShip->GetTowerPosition() - m_Src;
	wantedDir.z = 0.0f;
	wantedDir = glm::normalize( wantedDir );

	m_Dir = glm::normalize( m_Dir + wantedDir * delta * m_pOwner->GetInfo()->GetTracking() );
}

void Missile::UpdateGlow()
{
	const float size = GetGlowSize();
	glm::vec3 source = m_Src - glm::vec3( size / 2.0f, 0.0f, 0.0f );
	source -= m_Dir * size / 10.0f;

	Sprite sprite(
		source,
		GetGlowColour(),
		size,
		0 );

	//g_pGame->GetCurrentSector()->GetSpriteManager()->AddSprite( sprite );
}

void Missile::Render()
{
    using namespace glm;

    m_Angle = atan2f( m_Dir.y, m_Dir.x );

    mat4 translation = translate( m_Src );
    mat4 rotation = rotate( mat4( 1.0f ), m_Angle + 90.0f * Genesis::kDegToRad, vec3( 0.0f, 0.0f, 1.0f ) );

    m_pModel->Render( translation * rotation );
}

void Missile::Kill()
{
	Ammo::Kill();

	// We want to destroy the missile but leave the trail to fade, so we orphan it so the
	// TrailManager handles its ownership. The trail will only be destroyed when it no
	// longer has any valid points.
    TrailSharedPtr pTrail = m_pTrail.lock();
	if (pTrail)
	{
		pTrail->SetOrphan();
        pTrail = nullptr;
	}
}

}