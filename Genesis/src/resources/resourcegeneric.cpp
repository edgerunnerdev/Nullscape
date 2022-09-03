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

#include "resources/resourcegeneric.h"

namespace Genesis
{

//////////////////////////////////////////////////////////////////////////////
// ResourceGeneric
//////////////////////////////////////////////////////////////////////////////

ResourceGeneric::ResourceGeneric(const Filename& filename)
    : m_State(ResourceState::Unloaded)
    , m_Filename(filename)
    , m_Type(ResourceType::Unknown)
{
}

void ResourceGeneric::RegisterForgeRebuildCallback(void* pCaller, ForgeRebuildCallbackFn pCallback) 
{
    m_ForgeRebuildCallbacks.emplace_back(pCaller, pCallback);
}

void ResourceGeneric::UnregisterForgeRebuildCallback(void* pCaller) 
{
    m_ForgeRebuildCallbacks.remove_if(
        [pCaller](const ForgeRebuildCallbackInfo& info)
        {
            return info.first == pCaller;
        });
}

} // namespace Genesis