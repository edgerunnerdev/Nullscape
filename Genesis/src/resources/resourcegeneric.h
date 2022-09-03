// Copyright 2022 Pedro Nunes
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

#include <atomic>
#include <functional>
#include <list>
#include <utility>

namespace Genesis
{

//////////////////////////////////////////////////////////////////////////////
// ResourceGeneric
//////////////////////////////////////////////////////////////////////////////

using ForgeRebuildCallbackFn = std::function<void()>;

class ResourceGeneric
{
public:
    ResourceGeneric(const Filename& filename);
    virtual ~ResourceGeneric() {}
    virtual void Preload(){};
    virtual bool Load() = 0;
    virtual ResourceType GetType() const;
    virtual bool OnForgeRebuild();

    ResourceState GetState() const;
    void SetState(ResourceState state);
    const Filename& GetFilename() const;

    void RegisterForgeRebuildCallback(void* pCaller, ForgeRebuildCallbackFn pCallback);
    void UnregisterForgeRebuildCallback(void* pCaller);

protected:
    std::atomic<ResourceState> m_State;

private:
    Filename m_Filename;
    ResourceType m_Type;
    
    using ForgeRebuildCallbackInfo = std::pair<void*, ForgeRebuildCallbackFn>;
    std::list<ForgeRebuildCallbackInfo> m_ForgeRebuildCallbacks;
};

inline ResourceState ResourceGeneric::GetState() const
{
    return m_State;
}
inline void ResourceGeneric::SetState(ResourceState state)
{
    m_State = state;
}
inline const Filename& ResourceGeneric::GetFilename() const
{
    return m_Filename;
}
inline ResourceType ResourceGeneric::GetType() const
{
    return m_Type;
}

inline bool ResourceGeneric::OnForgeRebuild()
{
    for (auto& pCallbacks : m_ForgeRebuildCallbacks)
    {
        pCallbacks.second();
    }

    return true;
}

} // namespace Genesis
