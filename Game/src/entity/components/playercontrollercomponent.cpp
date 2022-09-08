// Copyright 2022 Pedro Nunes
//
// This file is part of Nullscape.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#include "entity/components/playercontrollercomponent.hpp"

#include "entity/components/navigationcomponent.hpp"
#include "entity/entity.hpp"
#include "sector/sector.h"
#include "sector/sectorcamera.h"
#include "game.hpp"

#include <genesis.h>

namespace Nullscape
{

PlayerControllerComponent::PlayerControllerComponent()
    : m_MoveToken(Genesis::InputManager::sInvalidInputCallbackToken)
    , m_MoveTimer(0.0f)
{
    using namespace Genesis;
    m_MoveToken = FrameWork::GetInputManager()->AddMouseCallback(
        [this]()
        {
            OnLeftMouseButtonDown();
        },
        MouseButton::Left, ButtonState::Pressed);
}

PlayerControllerComponent::~PlayerControllerComponent()
{
    using namespace Genesis;
    InputManager* pInputManager = FrameWork::GetInputManager();
    if (pInputManager)
    {
        pInputManager->RemoveKeyboardCallback(m_MoveToken);
    }
}

void PlayerControllerComponent::Update(float delta)
{
    if (m_MoveTimer > 0.0f)
    {
        m_MoveTimer -= delta;
    }
}

bool PlayerControllerComponent::Serialize(nlohmann::json& data)
{
    return false; // The PlayerControllerComponent should never be serialized. It must be manually added instead.
}

bool PlayerControllerComponent::Deserialize(const nlohmann::json& data)
{
    return false; // The PlayerControllerComponent should never be serialized. It must be manually added instead.
}

void PlayerControllerComponent::CloneFrom(Component* pComponent)
{
    SDL_assert(false); // The PlayerControllerComponent should never be cloned.
}

void PlayerControllerComponent::OnLeftMouseButtonDown() 
{
    if (m_MoveTimer > 0.0f) // On double click.
    {
        Sector* pSector = g_pGame->GetCurrentSector();
        if (pSector && pSector->GetCamera())
        {
            const glm::mat4x4& cameraTransform = pSector->GetCamera()->GetTransform();

            NavigationComponent* pNavigationComponent = GetOwner()->GetComponent<NavigationComponent>();
            if (pNavigationComponent)
            {
                pNavigationComponent->FlyTowards(glm::vec3(cameraTransform[0]));
            }
        }
    }
    else
    {
        m_MoveTimer = 0.5f;
    }
}

} // namespace Nullscape
