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
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <string>

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/traits/string.h>
#include <externalheadersend.hpp>
// clang-format on

#include "entity/component.hpp"

namespace Genesis
{
class ResourceModel;
}

namespace Nullscape
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

    template <typename S> void serialize(S& s) 
    {
        s.value2b(m_Version);
        s.text1b(m_Filename, 256);
    }

    DEFINE_COMPONENT(ModelComponent);

private:
    uint16_t m_Version;
    std::string m_Filename;
    Genesis::ResourceModel* m_pModel;
};

} // namespace Nullscape
