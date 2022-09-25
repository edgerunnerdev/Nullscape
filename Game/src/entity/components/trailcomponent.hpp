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

#include <memory>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "entity/component.hpp"

namespace Nullscape
{

GENESIS_DECLARE_SMART_PTR(Trail);
class SpriteManager;
class TrailManager;

class TrailComponent : public Component
{
public:
    TrailComponent();
    virtual ~TrailComponent() override;

    virtual void Initialize() override {}
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override;
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;
    virtual void OnAddedToScene(Genesis::Scene* pScene);
    virtual void OnRemovedFromScene();

    DEFINE_COMPONENT(TrailComponent);

private:
    TrailWeakPtr m_pTrail;
    glm::vec3 m_Offset;
    float m_Width;
    float m_Lifetime;
    glm::vec4 m_Color;
    bool m_DebugRender;
    SpriteManager* m_pSpriteManager;
    TrailManager* m_pTrailManager;
};

} // namespace Nullscape
