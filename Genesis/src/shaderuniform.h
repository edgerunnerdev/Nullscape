// Copyright 2016 Pedro Nunes
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

#include "rendersystem.fwd.h"
#include "resources/resourceimage.h"
#include "shaderuniformtype.h"

#include <glm/gtx/transform.hpp>
#include <vector>

namespace Genesis
{

///////////////////////////////////////////////////////////////////////////////
// ShaderUniform
///////////////////////////////////////////////////////////////////////////////

class ShaderUniform
{
public:
    ShaderUniform(GLuint handle, ShaderUniformType type, bool allowInstancingOverride = true, size_t count = 1);
    ~ShaderUniform();

    void Apply();

    void Set(bool value);
    void Set(int value);
    void Set(const std::vector<int>& values);
    void Set(float value);
    void Set(const std::vector<float>& values);
    void Set(const glm::vec2& value);
    void Set(const std::vector<glm::vec2>& values);
    void Set(const glm::vec3& value);
    void Set(const std::vector<glm::vec3>& values);
    void Set(const glm::vec4& value);
    void Set(const std::vector<glm::vec4>& values);
    void Set(const glm::mat4& value);
    void Set(const std::vector<glm::mat4>& values);
    void Set(ResourceImage* pTexture, GLenum textureSlot);
    void Set(GLuint textureID, GLenum textureSlot);

    void AllowInstancingOverride(bool state);
    bool IsInstancingOverrideAllowed() const;

    GLuint GetHandle() const;
    ShaderUniformType GetType() const;

private:
    GLuint m_Handle;
    ShaderUniformType m_Type;
    GLenum m_Slot;
    bool m_InstancingOverride;
    std::vector<uint8_t> m_Data;
    size_t m_Count;
};

inline void ShaderUniform::Set(bool value)
{
    SDL_assert(m_Type == ShaderUniformType::Boolean);
    *(bool*)m_Data.data() = value;
}

inline void ShaderUniform::Set(int value)
{
    SDL_assert(m_Type == ShaderUniformType::Integer);
    *(int*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<int>& values)
{
    SDL_assert(m_Type == ShaderUniformType::Integer);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(int) * m_Count);
}

inline void ShaderUniform::Set(float value)
{
    SDL_assert(m_Type == ShaderUniformType::Float);
    *(float*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<float>& values)
{
    SDL_assert(m_Type == ShaderUniformType::Float);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(float) * m_Count);
}

inline void ShaderUniform::Set(const glm::vec2& value)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector2);
    *(glm::vec2*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<glm::vec2>& values)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector2);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(glm::vec2) * m_Count);
}

inline void ShaderUniform::Set(const glm::vec3& value)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector3);
    *(glm::vec3*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<glm::vec3>& values)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector3);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(glm::vec3) * m_Count);
}

inline void ShaderUniform::Set(const glm::vec4& value)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector4);
    *(glm::vec4*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<glm::vec4>& values)
{
    SDL_assert(m_Type == ShaderUniformType::FloatVector4);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(glm::vec4) * m_Count);
}

inline void ShaderUniform::Set(const glm::mat4& value)
{
    SDL_assert(m_Type == ShaderUniformType::FloatMatrix44);
    *(glm::mat4*)m_Data.data() = value;
}

inline void ShaderUniform::Set(const std::vector<glm::mat4>& values)
{
    SDL_assert(m_Type == ShaderUniformType::FloatMatrix44);
    SDL_assert(m_Count == values.size());
    memcpy(m_Data.data(), values.data(), sizeof(glm::mat4) * m_Count);
}

inline void ShaderUniform::Set(ResourceImage* pImage, GLenum textureSlot)
{
    SDL_assert(m_Type == ShaderUniformType::Texture);
    *(GLuint*)m_Data.data() = pImage->GetTexture();
    m_Slot = textureSlot;
}

inline void ShaderUniform::Set(GLuint textureID, GLenum textureSlot)
{
    SDL_assert(m_Type == ShaderUniformType::Texture);
    *(GLuint*)m_Data.data() = textureID;
    m_Slot = textureSlot;
}

inline GLuint ShaderUniform::GetHandle() const
{
    return m_Handle;
}

inline ShaderUniformType ShaderUniform::GetType() const
{
    return m_Type;
}

inline void ShaderUniform::AllowInstancingOverride(bool state)
{
    SDL_assert(!(m_InstancingOverride == false && state == true));
    m_InstancingOverride = state;
}

inline bool ShaderUniform::IsInstancingOverrideAllowed() const
{
    return m_InstancingOverride;
}
} // namespace Genesis
