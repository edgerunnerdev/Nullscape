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

#include "shaderuniform.h"

#include "rendersystem.h"
#include "resources/resourceimage.h"

namespace Genesis
{

///////////////////////////////////////////////////////////////////////////////
// ShaderUniform
///////////////////////////////////////////////////////////////////////////////

ShaderUniform::ShaderUniform(GLuint handle, ShaderUniformType type, bool allowInstancingOverride /* = true */, size_t count /* = 1 */)
    : m_Handle(handle)
    , m_Type(type)
    , m_Slot(GL_TEXTURE0)
    , m_InstancingOverride(allowInstancingOverride)
    , m_Count(count)
{
    SDL_assert(handle != -1);
    SDL_assert(count > 0);

    if (m_Type == ShaderUniformType::Boolean)
    {
        SDL_assert(count == 1);
        m_Data.resize(1);
    }
    else if (m_Type == ShaderUniformType::Integer)
    {
        m_Data.resize(sizeof(int) * count);
    }
    else if (m_Type == ShaderUniformType::Float)
    {
        m_Data.resize(sizeof(float) * count);
    }
    else if (m_Type == ShaderUniformType::FloatVector2)
    {
        m_Data.resize(sizeof(glm::vec2) * count);
    }
    else if (m_Type == ShaderUniformType::FloatVector3)
    {
        m_Data.resize(sizeof(glm::vec3) * count);
    }
    else if (m_Type == ShaderUniformType::FloatVector4)
    {
        m_Data.resize(sizeof(glm::vec4) * count);
    }
    else if (m_Type == ShaderUniformType::FloatMatrix44)
    {
        m_Data.resize(sizeof(glm::mat4) * count);
    }
    else if (m_Type == ShaderUniformType::Texture)
    {
        SDL_assert(count == 1);
        m_Data.resize(sizeof(GLuint), 1);
    }
}

ShaderUniform::~ShaderUniform()
{

}

void ShaderUniform::Apply()
{
    if (m_Type == ShaderUniformType::Boolean)
    {
        const int v = (*(bool*)m_Data.data()) ? 1 : 0;
        glUniform1i(m_Handle, v);
    }
    else if (m_Type == ShaderUniformType::Integer)
    {
        glUniform1iv(m_Handle, m_Count, reinterpret_cast<GLint*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::Float)
    {
        glUniform1fv(m_Handle, m_Count, reinterpret_cast<GLfloat*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::FloatVector2)
    {
        glUniform2fv(m_Handle, m_Count, reinterpret_cast<GLfloat*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::FloatVector3)
    {
        glUniform3fv(m_Handle, m_Count, reinterpret_cast<GLfloat*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::FloatVector4)
    {
        glUniform4fv(m_Handle, m_Count, reinterpret_cast<GLfloat*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::FloatMatrix44)
    {
        glUniformMatrix4fv(m_Handle, m_Count, GL_FALSE, reinterpret_cast<GLfloat*>(m_Data.data()));
    }
    else if (m_Type == ShaderUniformType::Texture)
    {
        glActiveTexture(m_Slot);
        glBindTexture(GL_TEXTURE_2D, *(GLuint*)m_Data.data());
        glUniform1i(m_Handle, (int)(m_Slot - GL_TEXTURE0));
    }
    else
    {
        SDL_assert(false); // ShaderUniformType enum entry not implemented
    }
}
} // namespace Genesis
