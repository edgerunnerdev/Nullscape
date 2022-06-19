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

#include "coredefines.h"

#include <string>
#include <vector>

namespace Genesis
{

class ResourceImage;
class Shader;
class ShaderUniformInstance;

using ResourceImages = std::vector<ResourceImage*>;
using ShaderUniformInstances = std::vector<ShaderUniformInstance>;

class Material
{
public:
    Material();
    ~Material();

    const std::string& GetName() const;
    void SetName(const std::string& name);
    void SetShader(Shader* pShader);
    Shader* GetShader() const;
    ResourceImages& GetResourceImages();
    const ResourceImages& GetResourceImages() const;
    ShaderUniformInstances& GetShaderUniformInstances();
    const ShaderUniformInstances& GetShaderUniformInstances() const;

private:
    std::string m_Name;
    Shader* m_pShader;
    ResourceImages m_Resources;
    ShaderUniformInstances m_Uniforms;
};
GENESIS_DECLARE_SMART_PTR(Material);

inline Material::Material() {}
inline Material::~Material() {}

inline const std::string& Material::GetName() const
{
    return m_Name;
}

inline void Material::SetName(const std::string& name)
{
    m_Name = name;
}

inline void Material::SetShader(Shader* pShader)
{
    m_pShader = pShader;
}

inline Shader* Material::GetShader() const
{
    return m_pShader;
}

inline ResourceImages& Material::GetResourceImages()
{
    return m_Resources;
}

inline const ResourceImages& Material::GetResourceImages() const
{
    return m_Resources;
}

inline ShaderUniformInstances& Material::GetShaderUniformInstances()
{
    return m_Uniforms;
}

inline const ShaderUniformInstances& Material::GetShaderUniformInstances() const
{
    return m_Uniforms;
}

} // namespace Genesis
