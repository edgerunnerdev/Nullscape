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

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <string>

#include <coredefines.h>

#include "componenttype.hpp"

namespace Nullscape
{

GENESIS_DECLARE_SMART_PTR(Component)

#define DEFINE_COMPONENT(COMPONENT_NAME) \
    static const ComponentType sType = ComponentType::COMPONENT_NAME; \
    virtual ComponentType GetType() const override { return sType; }

class Entity;

class Component
{
public:
    Component()
        : m_pEntity(nullptr)
        , m_Version(1)
    {
    }

    virtual ~Component() {}

    virtual ComponentType GetType() const = 0;
    virtual void Initialize() = 0;
    virtual void Update(float delta) = 0;
    virtual void UpdateDebugUI() = 0;
    virtual void Render() = 0;

    virtual bool Serialize(nlohmann::json& data);
    virtual bool Deserialize(const nlohmann::json& data);
    virtual void CloneFrom(Component* pComponent);

    void SetOwner(Entity* pEntity);
    Entity* GetOwner() const;
    void SetName(const std::string& name);
    const std::string& GetName() const;
    int GetVersion() const;

protected:
    template <typename T>
    bool TryDeserialize(const nlohmann::json& data, const std::string& name, T& out) 
    {
        using namespace nlohmann;
        json::const_iterator it = data.find(name);
        if (it == data.cend())
        {
            return false;
        }
        else
        {
            out = it->get<T>();
            return true;
        }
    }

    void SetVersion(int version);

private:
    Entity* m_pEntity;
    std::string m_Name;
    int m_Version;
};

inline void Component::SetOwner(Entity* pEntity)
{
    m_pEntity = pEntity;
}

inline Entity* Component::GetOwner() const 
{
    return m_pEntity;
}

inline void Component::SetName(const std::string& name) 
{
    m_Name = name;
}

inline const std::string& Component::GetName() const 
{
    return m_Name;
}

inline void Component::SetVersion(int version)
{
    m_Version = version;
}

inline int Component::GetVersion() const 
{
    return m_Version;
}

} // namespace Nullscape
