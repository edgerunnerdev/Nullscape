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

#include <string>

#include "entity/component.hpp"

namespace Genesis
{
class ResourceModel;
}

namespace Hyperscape
{

class ModelComponent : public Component
{
public:
    ModelComponent();
    virtual ~ModelComponent() override;

    virtual void Initialize() override;
    virtual void Update(float delta) override;
    virtual void UpdateDebugUI() override;
    virtual void Render() override;
    virtual bool Serialize(nlohmann::json& data) override;
    virtual bool Deserialize(const nlohmann::json& data) override;
    virtual void CloneFrom(Component* pComponent) override;

    DEFINE_COMPONENT(ModelComponent);

private:
    std::string m_Filename;
    Genesis::ResourceModel* m_pModel;
};

} // namespace Hyperscape
