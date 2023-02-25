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

#include "entity/component.hpp"

#include <genesis.h>

namespace Hyperscape
{

bool Component::Serialize(nlohmann::json& data) 
{
    data["type"] = magic_enum::enum_name(GetType());
    data["version"] = GetVersion();

    if (!GetName().empty())
    {
        data["name"] = GetName();
    }

    return true;
}

bool Component::Deserialize(const nlohmann::json& data) 
{
    // Type is intentionally not deserialized here, but left to the ComponentFactory to handle.
    bool success = TryDeserialize(data, "version", m_Version);
    TryDeserialize(data, "name", m_Name); // "name" is optional.
    return success;
}

void Component::CloneFrom(Component* pComponent) 
{
    m_Version = pComponent->m_Version;
    m_Name = pComponent->m_Name;
}

} // namespace Hyperscape
