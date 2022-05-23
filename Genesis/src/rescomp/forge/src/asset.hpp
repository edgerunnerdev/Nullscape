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

#include <filesystem>
#include <string>

namespace Genesis
{
namespace ResComp
{

class Asset
{
public:
    Asset(const std::filesystem::path& path);

    bool IsValid() const;
    const std::string& GetCompiler() const;

private:
    std::filesystem::path m_Path;
    bool m_IsValid;
    std::string m_Compiler;
};

} // namespace ResComp
} // namespace Genesis
