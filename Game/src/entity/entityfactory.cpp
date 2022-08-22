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

#include "entity/entityfactory.hpp"

#include <fstream>
#include <memory>

#include <log.hpp>
#include <genesis.h>

#include "entity/component.hpp"
#include "entity/componentserialization.hpp"
#include "entity/entity.hpp"

namespace Nullscape
{

std::unique_ptr<EntityFactory> g_pEntityFactory;

EntityFactory::EntityFactory() 
{
    for (const auto& dirEntry : std::filesystem::directory_iterator("data/templates"))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".json")
        {
            LoadTemplate(dirEntry.path());
        }
    }
}

EntityFactory::~EntityFactory() 
{

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
    else
    {
        EntitySharedPtr pEntity = std::make_shared<Entity>();
        return pEntity;
    }

    //EntityTemplate const& entityTemplate = it->second;
    //EntitySharedPtr pEntity = std::make_shared<Entity>();

    //SerializationContext context{};
    //std::get<1>(context).registerBasesList<Deserializer>(ComponentPolymorphicClasses{});
    //Deserializer deserializer{context, entityTemplate.begin(), entityTemplate.size()};
    //deserializer.object(*pEntity);

    //if (deserializer.adapter().error() != bitsery::ReaderError::NoError || !deserializer.adapter().isCompletedSuccessfully())
    //{
    //    Genesis::Log::Error() << "Failed to create entity from template " << templateName << ": " << magic_enum::enum_name(deserializer.adapter().error());
    //    return nullptr;
    //}
    //else if (!std::get<0>(context).isValid())
    //{
    //    Genesis::Log::Error() << "Failed to create entity from template " << templateName << ": dangling pointers.";
    //    return nullptr;
    //}
    //else
    //{
    //    for (Component* pComponent : pEntity->GetComponents())
    //    {
    //        pComponent->SetOwner(pEntity.get());
    //        pComponent->Initialize();
    //    }

    //    return pEntity;
    //}
}

bool EntityFactory::AddBlankTemplate(const std::string& templateName) 
{
    if (m_Templates.find(templateName) != m_Templates.end())
    {
        return false;
    }

    SaveTemplate(templateName, std::make_shared<Entity>());
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
    using namespace nlohmann;

    std::ifstream file(path, std::ios::in);
    if (file.good())
    {
        json data = json::parse(file);

        //file.seekg(0, file.end);
        //EntityTemplate buffer;
        //std::string templateName = path.stem().generic_string();
        //size_t length = static_cast<size_t>(file.tellg());
        //if (length > 0)
        //{
        //    file.seekg(0, file.beg);
        //    buffer.resize(length);
        //    file.read(reinterpret_cast<char*>(buffer.data()), length);
        //    m_Templates[templateName] = buffer;
        //}
        //else
        //{
        //    AddBlankTemplate(templateName);
        //}

        file.close();
    }
    else
    {
        Genesis::Log::Error() << "Failed to load entity template " << path;
    }
}

void EntityFactory::SaveTemplate(const std::string& templateName, EntitySharedPtr pEntity) 
{
    using namespace Genesis;

    m_Templates[templateName] = pEntity;

    // Serialize the entity.
    nlohmann::json jData;
    if (!pEntity->Serialize(jData))
    {
        Log::Error() << "Failed to serialize entity '" << templateName << "'.";
        return;
    }

    // Store to disk.
    std::filesystem::path templatesPath("data/templates");
    std::string filename = templateName + ".json";
    std::ofstream file(templatesPath / filename);
    if (file.good())
    {
        file << std::setw(4) << jData << std::endl;
        file.close();
        Log::Info() << "Saved template '" << templateName << "' to '" << filename << "'.";
    }
    else
    {
        Log::Error() << "Failed to save template '" << templateName << "' to '" << filename << "'.";
    }
}

} // namespace Nullscape
