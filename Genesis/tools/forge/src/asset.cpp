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

#include <fstream>
#include <json.hpp>

namespace Genesis
{
namespace ResComp
{

Asset::Asset(const std::filesystem::path& path)
    : m_Path(path)
{
    using namespace nlohmann;
    std::ifstream file(path);
    if (file.good())
    {
        json j;
        file >> j;

        json::iterator it = j.find("compiler");
        if (it != j.end() && it->is_string())
        {
            m_Compiler = it->get<std::string>();
        }
        else
        {
            m_Compiler = "GenericComp";
        }

        file.close();
    }
}

const std::filesystem::path& Asset::GetPath() const
{
    return m_Path;
}

const std::string& Asset::GetCompiler() const
{
    return m_Compiler;
}

} // namespace ResComp
} // namespace Genesis
