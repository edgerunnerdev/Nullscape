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

#include "cache.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <fstream>
#include "asset.hpp"
#include "filehash.hpp"

namespace Genesis::ResComp
{

Cache::Cache(const std::filesystem::path& intermediatesDir) 
{
    using namespace nlohmann;

    m_CachePath = intermediatesDir / "cache.json";

    if (std::filesystem::exists(m_CachePath))
    {
        std::ifstream file(m_CachePath);
        if (file.good())
        {
            json j;
            j << file;
            file.close();

            m_Cache = j["cache"];
        }
    }
}

Cache::~Cache() 
{
    using namespace nlohmann;
    std::ofstream file(m_CachePath, std::ios::out | std::ios::trunc);
    if (file.good())
    {
        json j;
        j["cache"] = m_Cache;
        file << j.dump();
        file.close();
    }
}

void Cache::Add(Asset* pAsset)
{
    m_Cache[pAsset->GetPath().generic_string()] = pAsset->GetHash();
}
    
bool Cache::NeedsRebuild(Asset* pAsset)
{ 
    auto it = m_Cache.find(pAsset->GetPath().generic_string());
    return (it == m_Cache.end()) || (it->second != pAsset->GetHash());
}

} // namespace Genesis::ResComp
