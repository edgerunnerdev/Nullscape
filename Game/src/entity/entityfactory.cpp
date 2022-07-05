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

#include "entity/entityfactory.hpp"

#include <fstream>
#include <memory>

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <externalheadersend.hpp>
// clang-format on

#include <log.hpp>

#include "entity/component.hpp"
#include "entity/entity.hpp"

namespace Hyperscape
{

std::unique_ptr<EntityFactory> g_pEntityFactory;

EntityFactory::EntityFactory() 
{
    for (const auto& dirEntry : std::filesystem::directory_iterator("data/templates"))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".bin")
        {
            LoadTemplate(dirEntry.path());
        }
    }
}

EntityFactory::~EntityFactory() 
{
    std::filesystem::path templatesPath("data/templates");
    for (auto& pair : m_Templates)
    {
        std::string filename = pair.first + ".bin";
        std::ofstream file(templatesPath / filename, std::ifstream::binary);
        if (file.good())
        {
            file.write(reinterpret_cast<char*>(pair.second.data()), pair.second.size());
            file.close();        
        }
    }
}

EntityFactory* EntityFactory::Get() 
{
	if (g_pEntityFactory == nullptr)
    {
        g_pEntityFactory = std::make_unique<EntityFactory>();
    }

    return g_pEntityFactory.get();
}

EntitySharedPtr EntityFactory::Create(const std::string& templateName) const 
{
    auto it = m_Templates.find(templateName);
    if (it == m_Templates.end())
    {
        return nullptr;
    }

    EntitySharedPtr pEntity = std::make_shared<Entity>();

    // same as serialization, but returns deserialization state as a pair
    // first = error code, second = is buffer was successfully read from begin to the end.
    using InputAdapter = bitsery::InputBufferAdapter<EntityTemplate>;
    auto state = bitsery::quickDeserialization<InputAdapter>({it->second.begin(), it->second.size()}, *pEntity);

    if (state.first == bitsery::ReaderError::NoError && state.second)
    {
        return pEntity;
    }
    else
    {
        Genesis::Core::Log::Error() << "Failed to create entity from template " << templateName;
        return nullptr;
    }
}

bool EntityFactory::AddBlankTemplate(const std::string& templateName) 
{
    if (m_Templates.find(templateName) != m_Templates.end())
    {
        return false;
    }

    EntityTemplate buffer;
    using OutputAdapter = bitsery::OutputBufferAdapter<EntityTemplate>;
    size_t written = bitsery::quickSerialization<OutputAdapter>(buffer, Entity());
    if (written != 0)
    {
        buffer.resize(written);
    }

    m_Templates[templateName] = buffer;
    return true;
}

std::set<std::string> EntityFactory::GetTemplateNames() const 
{
    std::set<std::string> templateNames;
    for (auto& pair : m_Templates)
    {
        templateNames.insert(pair.first);
    }
    return templateNames;
}

void EntityFactory::LoadTemplate(const std::filesystem::path& path) 
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (file.good())
    {
        file.seekg(0, file.end);
        EntityTemplate buffer;
        std::string templateName = path.stem().generic_string();
        size_t length = static_cast<size_t>(file.tellg());
        if (length > 0)
        {
            file.seekg(0, file.beg);
            buffer.resize(length);
            file.read(reinterpret_cast<char*>(buffer.data()), length);
            m_Templates[templateName] = buffer;
        }
        else
        {
            AddBlankTemplate(templateName);
        }

        file.close();
    }
    else
    {
        Genesis::Core::Log::Error() << "Failed to load entity template " << path;
    }
}

} // namespace Hyperscape
