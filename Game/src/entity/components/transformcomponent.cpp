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

#include "entity/components/transformcomponent.hpp"

#include <genesis.h>
#include <imgui/imgui.h>
#include <sstream>

namespace Hyperscape
{

TransformComponent::TransformComponent()
    : m_Version(1)
    , m_Transform(1)
{
}

void TransformComponent::UpdateDebugUI()
{
    for (int i = 0; i < 4; ++i)
    {
        float v[4] = {m_Transform[i][0], m_Transform[i][1], m_Transform[i][2], m_Transform[i][3]};
        std::stringstream label;
        label << "Row " << i;
        if (ImGui::InputFloat4(label.str().c_str(), v))
        {
            m_Transform[i] = glm::vec4(v[0], v[1], v[2], v[3]);
        }
    }
}

bool TransformComponent::Serialize(nlohmann::json& data)
{
    bool success = Component::Serialize(data);
    return success;
}

bool TransformComponent::Deserialize(const nlohmann::json& data)
{
    bool success = Component::Deserialize(data);
    return success;
}

void TransformComponent::CloneFrom(Component* pComponent)
{
    Component::CloneFrom(pComponent);
}

} // namespace Hyperscape
