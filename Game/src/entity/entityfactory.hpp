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
#include <vector>

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
    void SaveTemplate(const std::string& templateName, EntitySharedPtr pEntity);

private:
    void LoadTemplate(const std::filesystem::path& path);

    std::unordered_map<std::string, EntitySharedPtr> m_Templates;
};

} // namespace Hyperscape
