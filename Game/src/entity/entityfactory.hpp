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

#include <filesystem>
#include <unordered_map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/ext/pointer.h>
#include <bitsery/ext/inheritance.h>
#include <externalheadersend.hpp>
// clang-format on

#include <coredefines.h>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Entity)

class EntityFactory
{
public:
    EntityFactory();
    ~EntityFactory();

    static EntityFactory* Get();

    EntitySharedPtr Create(const std::string& templateName) const;
    bool AddBlankTemplate(const std::string& templateName);
    std::set<std::string> GetTemplateNames() const;
    void SaveTemplate(const std::string& templateName, Entity* pEntity);

private:
    void LoadTemplate(const std::filesystem::path& path);

    using EntityTemplate = std::vector<uint8_t>;
    using SerializationWriter = bitsery::OutputBufferAdapter<EntityTemplate>;
    using SerializationReader = bitsery::InputBufferAdapter<EntityTemplate>;
    using SerializationContext = std::tuple<bitsery::ext::PointerLinkingContext, bitsery::ext::PolymorphicContext<bitsery::ext::StandardRTTI>>;
    using Serializer = bitsery::Serializer<SerializationWriter, SerializationContext>;
    using Deserializer = bitsery::Deserializer<SerializationReader, SerializationContext>;

    std::unordered_map<std::string, EntityTemplate> m_Templates;
};

} // namespace Hyperscape
