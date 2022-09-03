// Copyright 2014 Pedro Nunes
//
// This file is part of Genesis.
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

#include "filename.h"
#include "resources/resourcetypes.h"
#include "taskmanager.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Genesis
{

class ForgeListener;
class ResourceGeneric;

typedef std::function<ResourceGeneric*(const Filename&)> ResourceFactoryFunction;

//////////////////////////////////////////////////////////////////////////////
// ExtensionData
//////////////////////////////////////////////////////////////////////////////

class ExtensionData
{
public:
    ExtensionData(const std::string& name, ResourceFactoryFunction& func)
        : m_Name(name)
        , m_ResourceFactoryFunction(func)
    {
    }

    const std::string& GetName() const;
    ResourceFactoryFunction GetFactoryFunction() const;

private:
    std::string m_Name; // "obj", "tga"...
    ResourceFactoryFunction m_ResourceFactoryFunction;
};

inline const std::string& ExtensionData::GetName() const
{
    return m_Name;
}
inline ResourceFactoryFunction ExtensionData::GetFactoryFunction() const
{
    return m_ResourceFactoryFunction;
}

using ExtensionMap = std::unordered_map<std::string, ExtensionData*>;
using ResourceMap = std::unordered_map<std::string, ResourceGeneric*>;

//////////////////////////////////////////////////////////////////////////////
// ResourceManager
//////////////////////////////////////////////////////////////////////////////

class ResourceManager : public Task
{
public:
    ResourceManager();
    ~ResourceManager();
    void RegisterExtension(const std::string& extension, ResourceFactoryFunction& func);
    TaskStatus Update(float delta);

    bool CanLoadResource(const Filename& filename);

    // Retrieves a resource. This is a blocking operation.
    ResourceGeneric* GetResource(const Filename& filename);
    template <typename T> T GetResource(const Filename& filename) { return static_cast<T>(GetResource(filename)); }

private:
    ExtensionMap mRegisteredExtensions;
    ResourceMap mResources;
    std::unique_ptr<ForgeListener> m_pForgeListener;
};

} // namespace Genesis
