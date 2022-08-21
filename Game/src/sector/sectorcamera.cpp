// Copyright 2022 Pedro Nunes
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

#include "sector/sectorcamera.h"

#include "entity/components/transformcomponent.hpp"
#include "entity/entity.hpp"
#include "hexterminate.h"
#include "player.h"
#include "sector/sector.h"

#include <configuration.h>
#include <genesis.h>
#include <inputmanager.h>
#include <scene/scene.h>
#include <sound/soundmanager.h>
#include <window.h>

namespace Nullscape
{

SectorCamera::SectorCamera()
{
}

SectorCamera::~SectorCamera()
{
}

void SectorCamera::Update(float delta)
{
    Entity* pShip = g_pGame->GetCurrentSector()->GetPlayerShip();
    if (pShip == nullptr)
    {
        return;
    }

    TransformComponent* pShipTransform = pShip->GetComponent<TransformComponent>();

    if (pShipTransform != nullptr)
    {
        const glm::vec3 shipPosition(pShipTransform->GetPosition());

        static glm::vec3 from(-10.0f, 9.0f, 30.0f);
        static glm::vec3 to(20.0f, 9.0f, 0.0f);

        Genesis::Camera* pCamera = Genesis::FrameWork::GetScene()->GetCamera();
        pCamera->SetPosition(shipPosition + from);
        pCamera->SetTargetPosition(shipPosition + to);
    }

    UpdateListener(delta);
    UpdateBorders();
}

void SectorCamera::UpdateListener(float delta)
{
    using namespace Genesis;

    const Scene* pScene = FrameWork::GetScene();
    const Camera* pCamera = pScene->GetCamera();

    const glm::vec3& position = pCamera->GetPosition();
    glm::vec3 forward = pCamera->GetTargetPosition() - position;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 velocity(0.0f, 0.0f, 0.0f);

    FrameWork::GetSoundManager()->SetListener(position, velocity, glm::normalize(forward), up);
}

void SectorCamera::UpdateBorders()
{
    using namespace Genesis;
    const Camera* pCamera = FrameWork::GetScene()->GetCamera();

    const glm::vec3& position = pCamera->GetPosition();
    const float farDist = position.z;
    const float screenRatio = static_cast<float>(Configuration::GetScreenWidth()) / static_cast<float>(Configuration::GetScreenHeight());
    const float fovY = 45.0f;
    const float fovX = fovY * screenRatio;

    const float hh = tanf(fovY / 2.0f) * farDist;
    m_BorderTopLeft.y = position.y + hh;
    m_BorderBottomRight.y = position.y - hh;

    const float hw = hh * screenRatio;
    m_BorderTopLeft.x = position.x - hw;
    m_BorderBottomRight.x = position.x + hw;
}

void SectorCamera::GetBorders(glm::vec2& topLeft, glm::vec2& bottomRight) const
{
    topLeft = m_BorderTopLeft;
    bottomRight = m_BorderBottomRight;
}

} // namespace Nullscape