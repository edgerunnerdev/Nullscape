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

#include <array>
#include <string>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "entity/component.hpp"

namespace Hyperscape
{

class SwayComponent : public Component
{
public:
    SwayComponent();
    virtual ~SwayComponent() override {}

    virtual void Initialize() override {}
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override {}
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;

    DEFINE_COMPONENT(SwayComponent);

private:
    uint16_t m_Version;
    bool m_BaseTransformSet;
    glm::mat4x4 m_BaseTransform;
    float m_Amplitude;
    glm::vec3 m_Values;
};

} // namespace Hyperscape
