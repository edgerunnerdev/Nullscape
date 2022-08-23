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

#include "asset.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "compiler.hpp"
#include "filehash.hpp"
#include "forge.hpp"

#include <fstream>
#include <vector>
#include <xxhash64.h>

namespace Genesis
{
namespace ResComp
{

Asset::Asset(Forge* pForge, const std::filesystem::path& path)
    : m_IsValid(false)
    , m_Path(path)
{
    using namespace nlohmann;
    std::ifstream file(path);
    if (file.good())
    {
        bool errors = false;
        json j;
        file >> j;
        file.close();

        std::string compilerToFind("GenericComp");
        json::iterator it = j.find("compiler");
        if (it != j.end() && it->is_string())
        {
            compilerToFind = it->get<std::string>();
        }

        m_pCompiler = pForge->FindCompiler(compilerToFind);
        if (m_pCompiler == nullptr)
        {
            errors = true;
        }

        it = j.find("source");
        if (it != j.end() && it->is_string())
        {
            m_Source = it->get<std::string>();
        }
        else
        {
            errors = true;
        }

        m_IsValid = !errors;
    }
}

bool Asset::IsValid() const
{
    return m_IsValid;
}

const std::filesystem::path& Asset::GetPath() const
{
    return m_Path;
}

CompilerSharedPtr Asset::GetCompiler() const
{
    return m_pCompiler;
}

const std::string& Asset::GetSource() const
{
    return m_Source;
}

uint64_t Asset::GetHash() const
{
    std::vector<uint64_t> hashes;
    hashes.push_back(m_pCompiler->GetHash());
    hashes.push_back(CalculateFileHash(m_Path));
    hashes.push_back(CalculateFileHash(std::filesystem::path(m_Path).remove_filename() / m_Source));
    return XXHash64::hash(hashes.data(), sizeof(uint64_t) * hashes.size(), 0);
}

} // namespace ResComp
} // namespace Genesis
