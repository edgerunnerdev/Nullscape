// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
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
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#include "entity/components/playercontrollercomponent.hpp"

#include "entity/entity.hpp"
#include "sector/sector.h"
#include "game.hpp"

#include <genesis.h>

namespace Hyperscape
{

PlayerControllerComponent::PlayerControllerComponent()
{

}

PlayerControllerComponent::~PlayerControllerComponent()
{

}

void PlayerControllerComponent::Update(float delta)
{

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

} // namespace Hyperscape
