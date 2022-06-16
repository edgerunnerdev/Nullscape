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


#include "material.hpp"

namespace Genesis
{
namespace ResComp
{

Material::Material()
{

}

Material::~Material()
{

}

void Material::SetName(const std::string& name)
{
    m_Name = name;
}

const std::string& Material::GetName() const
{
    return m_Name;
}

void Material::SetShader(const std::string& shader)
{
    m_Shader = shader;
}

const std::string& Material::GetShader() const
{
    return m_Shader;
}

const Material::Bindings& Material::GetBindings() const
{
    return m_Bindings;
}
 
Material::Bindings& Material::GetBindings()
{
    return m_Bindings;
}

} // namespace ResComp
} // namespace Genesis
