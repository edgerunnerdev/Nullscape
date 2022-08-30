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
#include "game.hpp"
#include "player.h"
#include "sector/sector.h"

#include <configuration.h>
#include <genesis.h>
#include <scene/scene.h>
#include <sound/soundmanager.h>
#include <window.h>

namespace Nullscape
{

SectorCamera::SectorCamera()
    : m_Transform(1)
    , m_RightMouseButtonPressed(Genesis::InputManager::sInvalidInputCallbackToken)
    , m_RightMouseButtonReleased(Genesis::InputManager::sInvalidInputCallbackToken)
    , m_CameraOrbit(false)
    , m_Pitch(0.0f)
    , m_Yaw(0.0f)
{
    using namespace Genesis;
    m_RightMouseButtonPressed = FrameWork::GetInputManager()->AddMouseCallback(
        [this]()
        {
            m_CameraOrbit = true;
        },
        MouseButton::Right, ButtonState::Pressed);
    m_RightMouseButtonReleased = FrameWork::GetInputManager()->AddMouseCallback(
        [this]()
        {
            m_CameraOrbit = false;
        },
        MouseButton::Right, ButtonState::Released);
}

SectorCamera::~SectorCamera() 
{
    using namespace Genesis;
    InputManager* pInputManager = FrameWork::GetInputManager();
    if (pInputManager)
    {
        pInputManager->RemoveMouseCallback(m_RightMouseButtonPressed);
        pInputManager->RemoveMouseCallback(m_RightMouseButtonReleased);
    }
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
        const glm::vec3 shipDirection(pShipTransform->GetTransform()[0]);
        glm::vec3 shipDirectionXZ = glm::normalize(glm::vec3(shipDirection.x, 0.0f, shipDirection.z));

        if (m_CameraOrbit)
        {
            glm::vec2 mouseDelta = Genesis::FrameWork::GetInputManager()->GetMouseDelta();
            m_Yaw += mouseDelta.x;
            m_Pitch += mouseDelta.y;
        }

        m_Pitch = glm::clamp(m_Pitch, -60.0f, 60.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        direction.y = sin(glm::radians(m_Pitch));
        direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        static glm::vec3 offset(-20.0f, 6.0f, 8.0f);

        m_Transform = glm::translate(offset);

        glm::mat4x4 transformYaw = glm::rotate(glm::radians(m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4x4 transformPitch = glm::rotate(glm::radians(m_Pitch), glm::vec3(0.0f, 0.0f, 1.0f));
        m_Transform = transformYaw * transformPitch * glm::translate(offset);

        Genesis::Camera* pCamera = Genesis::FrameWork::GetScene()->GetCamera();

        glm::vec4 cameraPosition(m_Transform[3]);
        glm::vec4 cameraDirection(m_Transform[0]);

        pCamera->SetPosition(glm::vec3(cameraPosition));
        pCamera->SetTargetPosition(glm::vec3(cameraPosition + cameraDirection));
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