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

#include <fstream>
#include <log.hpp>

#include "render/viewport.hpp"
#include "resources/resourceshader.hpp"
#include "scene/light.h"
#include "scene/scene.h"
#include "genesis.h"
#include "shaderuniform.h"
#include "shaderuniforminstance.h"
#include "rendersystem.h"

namespace Genesis
{

ResourceShader::ResourceShader(const Filename& filename) 
    : ResourceGeneric(filename)
    , m_ProgramHandle(0)
    , m_pModelViewProjectionUniform(nullptr)
    , m_pModelUniform(nullptr)
    , m_pModelInverseUniform(nullptr)
    , m_pModelInverseTransposeUniform(nullptr)
    , m_pViewInverseUniform(nullptr)
    , m_pTimeUniform(nullptr)
    , m_pResolutionUniform(nullptr)
    , m_pLightPositionUniform(nullptr)
    , m_pLightColorUniform(nullptr)
{
    
}

ResourceShader::~ResourceShader()
{
    glDeleteProgram(m_ProgramHandle);
}

ResourceType ResourceShader::GetType() const 
{
    return ResourceType::Shader;
}

bool ResourceShader::Load()
{
    if (!CompileShader())
    {
        return false;
    }

    m_State = ResourceState::Loaded;
    return true;
}

bool ResourceShader::OnForgeBuild()
{
    return CompileShader();
}

bool ResourceShader::CompileShader() 
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string shaderCode;
    std::ifstream shaderFile(GetFilename().GetFullPath());
    m_ShaderName = GetFilename().GetName();

    if (shaderFile.is_open())
    {
        std::string line = "";
        while (getline(shaderFile, line))
        {
            shaderCode += "\n" + line;
        }

        shaderFile.close();
    }
    else
    {
        Log::Error() << "Couldn't open " << GetFilename().GetFullPath() << ".";
        return false;
    }

    Log::Info() << "Compiling shader program: " << m_ShaderName;
    const std::string vertexShaderCode = "#version 330\n#define VERTEX_PROGRAM\n#line 0\n" + shaderCode;
    const std::string fragmentShaderCode = "#version 330\n#define FRAGMENT_PROGRAM\n#line 0\n" + shaderCode;

    GLint compilationSuccessful = GL_FALSE;
    int infoLogLength = 0;

    // Compile Vertex Shader
    const char* vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, nullptr);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compilationSuccessful);
    if (compilationSuccessful == GL_FALSE)
    {
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, vertexShaderErrorMessage.data());
        Log::Error() << "Compiling shader '" << m_ShaderName << "':" << vertexShaderErrorMessage.data();
        return false;
    }

    // Compile Fragment Shader
    const char* fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, nullptr);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compilationSuccessful);
    if (compilationSuccessful == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, fragmentShaderErrorMessage.data());
        Log::Error() << "Compiling shader '" << m_ShaderName << "':" << fragmentShaderErrorMessage.data();
        return false;
    }

    // Link the program
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShaderID);
    glAttachShader(programHandle, fragmentShaderID);
    glLinkProgram(programHandle);

    // Check the program
    GLint linkSuccessful = GL_FALSE;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccessful);
    if (linkSuccessful == GL_FALSE)
    {
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programHandle, infoLogLength, nullptr, programErrorMessage.data());
        Log::Error() << programErrorMessage.data();
        return false;
    }

    glDetachShader(programHandle, vertexShaderID);
    glDetachShader(programHandle, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    m_ProgramHandle = programHandle;

    RegisterCoreUniforms();

    return true;
}

const std::string& ResourceShader::GetName() const 
{
    return m_ShaderName;
}

void ResourceShader::RegisterCoreUniforms()
{
    m_pModelViewProjectionUniform = RegisterUniform("k_worldViewProj", ShaderUniformType::FloatMatrix44);
    m_pModelUniform = RegisterUniform("k_world", ShaderUniformType::FloatMatrix44);
    m_pModelInverseUniform = RegisterUniform("k_worldInverse", ShaderUniformType::FloatMatrix44);
    m_pModelInverseTransposeUniform = RegisterUniform("k_worldInverseTranspose", ShaderUniformType::FloatMatrix44);
    m_pViewInverseUniform = RegisterUniform("k_viewInverse", ShaderUniformType::FloatMatrix44);
    m_pTimeUniform = RegisterUniform("k_time", ShaderUniformType::Float);
    m_pResolutionUniform = RegisterUniform("k_resolution", ShaderUniformType::FloatVector2);
    m_pLightPositionUniform = RegisterUniform("LightPosition", ShaderUniformType::FloatVector3, false, 3);
    m_pLightColorUniform = RegisterUniform("LightColor", ShaderUniformType::FloatVector3, false, 3);
}

ShaderUniformSharedPtr ResourceShader::RegisterUniform(const char* pUniformName, ShaderUniformType type, bool allowInstancingOverride /* = true */, size_t count /* = 1*/)
{
    GLuint handle = glGetUniformLocation(m_ProgramHandle, pUniformName);
    for (auto& pUniform : m_Uniforms)
    {
        if (pUniform->GetHandle() == handle)
        {
            if (pUniform->IsInstancingOverrideAllowed() == true && allowInstancingOverride == false)
            {
                pUniform->AllowInstancingOverride(false);
            }

            return pUniform;
        }
    }

    ShaderUniformSharedPtr pUniform = nullptr;
    if (handle != -1)
    {
        pUniform = std::make_shared<ShaderUniform>(handle, type, allowInstancingOverride, count);
        m_Uniforms.push_back(pUniform);
    }
    return pUniform;
}

void ResourceShader::Use(ShaderUniformInstances* pShaderUniformInstances /* = nullptr */)
{
    glm::mat4 modelMatrix(1.0f); // identity
    Use(modelMatrix, pShaderUniformInstances);
}

void ResourceShader::Use(const glm::mat4& modelMatrix, ShaderUniformInstances* pShaderUniformInstances /* = nullptr */)
{
    glUseProgram(m_ProgramHandle);

    UpdateParameters(modelMatrix, pShaderUniformInstances);
}

void ResourceShader::UpdateParameters(const glm::mat4& modelMatrix, ShaderUniformInstances* pShaderUniformInstances)
{
    RenderSystem* renderSystem = FrameWork::GetRenderSystem();
    Viewport* pViewport = renderSystem->GetCurrentViewport();
    SDL_assert(pViewport != nullptr);

    if (m_pModelViewProjectionUniform != nullptr)
    {
        const glm::mat4 mvp = renderSystem->GetProjectionMatrix() * renderSystem->GetViewMatrix() * modelMatrix;
        m_pModelViewProjectionUniform->Set(mvp);
    }

    if (m_pModelUniform != nullptr)
    {
        m_pModelUniform->Set(modelMatrix);
    }

    if (m_pModelInverseUniform != nullptr)
    {
        glm::mat3x3 modelMatrix3(modelMatrix);
        glm::mat3x3 modelInverseMatrix = glm::inverse(modelMatrix3);

        m_pModelInverseUniform->Set(glm::mat4(modelInverseMatrix));
    }

    if (m_pModelInverseTransposeUniform != nullptr)
    {
        glm::mat3x3 modelMatrix3(renderSystem->GetViewMatrix() * modelMatrix);
        glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(modelMatrix3));

        m_pModelInverseTransposeUniform->Set(glm::mat4(normalMatrix));
    }

    if (m_pViewInverseUniform != nullptr)
    {
        m_pViewInverseUniform->Set(glm::inverse(renderSystem->GetViewMatrix()));
    }

    if (m_pTimeUniform != nullptr)
    {
        m_pTimeUniform->Set(FrameWork::GetRenderSystem()->GetShaderTimer());
    }

    if (m_pResolutionUniform != nullptr)
    {
        m_pResolutionUniform->Set(glm::vec2(static_cast<float>(pViewport->GetWidth()), static_cast<float>(pViewport->GetHeight())));
    }

    if (m_pLightPositionUniform != nullptr)
    {
        const LightArray& lights = pViewport->GetScene()->GetLights();
        std::vector<glm::vec3> positions = {lights[0].GetPosition(), lights[1].GetPosition(), lights[2].GetPosition()};
        m_pLightPositionUniform->Set(positions);
    }

    if (m_pLightColorUniform != nullptr)
    {
        const LightArray& lights = pViewport->GetScene()->GetLights();
        std::vector<glm::vec3> colors = {lights[0].GetColor(), lights[1].GetColor(), lights[2].GetColor()};
        m_pLightColorUniform->Set(colors);
    }

    if (pShaderUniformInstances != nullptr)
    {
        for (auto& shaderUniformInstance : *pShaderUniformInstances)
        {
            shaderUniformInstance.Apply();
        }
    }

    for (auto& pUniform : m_Uniforms)
    {
        pUniform->Apply();
    }
}

} // namespace Genesis