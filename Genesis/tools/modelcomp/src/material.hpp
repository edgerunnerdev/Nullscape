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

#include <unordered_map>

namespace Genesis
{
namespace ResComp
{

class Material
{
public:
    Material();
    ~Material();

    void SetName(const std::string& name);
    const std::string& GetName() const;

    void SetShader(const std::string& shader);
    const std::string& GetShader() const;

    using Bindings = std::unordered_map<std::string, std::string>;
    const Bindings& GetBindings() const;
    Bindings& GetBindings();

private:
    std::string m_Name;
    std::string m_Shader;
    Bindings m_Bindings;
};

} // namespace ResComp
} // namespace Genesis
