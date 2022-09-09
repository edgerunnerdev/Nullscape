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

#pragma once

#include "entity/component.hpp"

#include <array>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/gtx/quaternion.hpp>
#include <externalheadersend.hpp>
// clang-format on

namespace Nullscape
{

class NavigationComponent : public Component
{
public:
    NavigationComponent();
    virtual ~NavigationComponent() override {}

    virtual void Initialize() override {}
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override {}
    virtual void Render() override {}
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;
    virtual bool UpdatesInEditor() const override;

    void FlyTowards(const glm::vec3& direction);

    DEFINE_COMPONENT(NavigationComponent);

private:
    enum class Mode
    {
        None,
        Direction
    };

    uint16_t m_Version;
    Mode m_Mode;

    glm::quat m_SourceRotation;
    glm::quat m_TargetRotation;
    float m_TargetInterpolation;
};

inline bool NavigationComponent::UpdatesInEditor() const
{
    return false;
}

} // namespace Nullscape
