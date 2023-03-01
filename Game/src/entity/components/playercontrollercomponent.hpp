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

#pragma once

#include "entity/component.hpp"

#include <genesis.h>

namespace Hyperscape
{

class PlayerControllerComponent : public Component
{
public:
    PlayerControllerComponent();
    virtual ~PlayerControllerComponent() override;

    virtual void Initialize() override {}
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override {}
    virtual void Render() override {}
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;
    virtual bool UpdatesInEditor() const override;

    DEFINE_COMPONENT(PlayerControllerComponent);

private:
};

inline bool PlayerControllerComponent::UpdatesInEditor() const
{
    return false;
}

} // namespace Hyperscape
