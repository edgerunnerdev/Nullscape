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

// clang-format off
#include <externalheadersbegin.hpp>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <SDL.h>
#include <externalheadersend.hpp>
// clang-format on

#include <vector>

#include "rendersystem.fwd.h"
#include "resources/resourcegeneric.h"
#include "resourcemanager.h"
#include "shaderuniformtype.h"

namespace Genesis
{

class ShaderParameters;
class ShaderUniform;
class ShaderUniformInstance;

using ShaderUniformInstances = std::vector<ShaderUniformInstance>;

class ResourceShader : public ResourceGeneric
{
public:
    ResourceShader(const Filename& filename);
    virtual ~ResourceShader() override;
    virtual ResourceType GetType() const override;
    virtual bool Load() override;
    virtual bool OnForgeRebuild() override;

    const std::string& GetName() const;

    void Use(ShaderUniformInstances* pShaderUniformInstances = nullptr);
    void Use(const glm::mat4& modelTransform, ShaderUniformInstances* pShaderUniformInstances = nullptr);

    ShaderUniformSharedPtr RegisterUniform(const char* pUniformName, ShaderUniformType type, bool allowInstancingOverride = true, size_t count = 1);

private:
    bool CompileShader();
    void RegisterCoreUniforms();

    void UpdateParameters(const glm::mat4& modelMatrix, ShaderUniformInstances* pShaderUniformInstances);

    std::string m_ShaderName;
    GLuint m_ProgramHandle;
    ShaderUniformSharedPtr m_pModelViewProjectionUniform;
    ShaderUniformSharedPtr m_pModelUniform;
    ShaderUniformSharedPtr m_pModelInverseUniform;
    ShaderUniformSharedPtr m_pModelInverseTransposeUniform;
    ShaderUniformSharedPtr m_pViewInverseUniform;
    ShaderUniformSharedPtr m_pTimeUniform;
    ShaderUniformSharedPtr m_pResolutionUniform;
    ShaderUniformSharedPtr m_pLightPositionUniform;
    ShaderUniformSharedPtr m_pLightColorUniform;
    std::vector<ShaderUniformSharedPtr> m_Uniforms;
};

} // namespace Genesis