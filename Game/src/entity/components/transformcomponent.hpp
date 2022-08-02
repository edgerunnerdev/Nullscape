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
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <string>

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/traits/string.h>
#include <glm/mat4x4.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "entity/component.hpp"

namespace Genesis
{
class ResourceModel;
}

namespace Hyperscape
{

class TransformComponent : public Component
{
public:
    TransformComponent();
    virtual ~TransformComponent() override {}

    virtual void Initialize() override {}
    virtual void Update(float delta) override {}
    virtual void UpdateDebugUI() override;
    virtual void Render() override {}

    const glm::mat4x4& GetTransform() const;
    void SetTransform(const glm::mat4x4& value);

    template <typename S> void serialize(S& s) 
    {
        s.value2b(m_Version);

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                s.value4b(m_Transform[i][j]);
            }
        }
    }

    DEFINE_COMPONENT(TransformComponent);

private:
    uint16_t m_Version;
    glm::mat4x4 m_Transform;
};

inline const glm::mat4x4& TransformComponent::GetTransform() const
{
    return m_Transform;
}
inline void TransformComponent::SetTransform(const glm::mat4x4& value)
{
    m_Transform = value;
}

} // namespace Hyperscape
