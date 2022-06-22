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

#include "vertexbuffer.h"

#include "genesis.h"
#include "rendersystem.h"

namespace Genesis
{

///////////////////////////////////////////////////////////////////////////////
// VertexBuffer
///////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer() 
{
}

VertexBuffer::VertexBuffer(GeometryType type, unsigned int flags)
    : m_Flags(flags)
    , m_VAO(0)
    , m_Position(0)
    , m_UV(0)
    , m_Normal(0)
    , m_Tangent(0)
    , m_Bitangent(0)
    , m_Colour(0)
    , m_Index(0)
    , m_Mode(GL_TRIANGLES)
{
    m_Size.fill(0);

    SDL_assert(flags & VBO_POSITION);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_Position);

    if (flags & VBO_UV)
    {
        glGenBuffers(1, &m_UV);
    }

    if (flags & VBO_NORMAL)
    {
        glGenBuffers(1, &m_Normal);
    }

    if (flags & VBO_COLOUR)
    {
        glGenBuffers(1, &m_Colour);
    }

    if (flags & VBO_TANGENT)
    {
        glGenBuffers(1, &m_Tangent);
    }

    if (flags & VBO_BITANGENT)
    {
        glGenBuffers(1, &m_Bitangent);
    }

    if (flags & VBO_INDEX)
    {
        glGenBuffers(1, &m_Index);
    }

    SetModeFromGeometryType(type);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_Position);
    glDeleteBuffers(1, &m_UV);
    glDeleteBuffers(1, &m_Normal);
    glDeleteBuffers(1, &m_Colour);
    glDeleteBuffers(1, &m_Tangent);
    glDeleteBuffers(1, &m_Bitangent);
    glDeleteBuffers(1, &m_Index);

    glDeleteVertexArrays(1, &m_VAO);
}

void VertexBuffer::SetModeFromGeometryType(GeometryType type)
{
    if (type == GeometryType::Triangle)
    {
        m_Mode = GL_TRIANGLES;
    }
    else if (type == GeometryType::Line)
    {
        m_Mode = GL_LINES;
    }
    else if (type == GeometryType::LineStrip)
    {
        m_Mode = GL_LINE_STRIP;
    }
    else
    {
        SDL_assert(false);
    }
}

void VertexBuffer::CreateUntexturedQuad(float x, float y, float width, float height)
{
    const float x1 = x;
    const float x2 = x + width;
    const float y1 = y;
    const float y2 = y + height;

    const float vertices[] = {
        x1, y2, 0.0f, x1, y1, 0.0f, x2, y1, 0.0f, x1, y2, 0.0f, x2, y1, 0.0f, x2, y2, 0.0f,
    };

    CopyData(vertices, 18, VBO_POSITION);
}

void VertexBuffer::CreateUntexturedQuad(float x, float y, float width, float height, const glm::vec4& colour)
{
    CreateUntexturedQuad(x, y, width, height);

    const float colours[] = {colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a,
                             colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a};

    CopyData(colours, 24, Genesis::VBO_COLOUR);
}

void VertexBuffer::CreateTexturedQuad(float x, float y, float width, float height)
{
    CreateUntexturedQuad(x, y, width, height);

    const float uvs[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    CopyData(uvs, 12, Genesis::VBO_UV);
}

void VertexBuffer::CreateTexturedQuad(float x, float y, float width, float height, const glm::vec4& colour)
{
    CreateTexturedQuad(x, y, width, height);

    const float colours[] = {colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a,
                             colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a, colour.r, colour.g, colour.b, colour.a};

    CopyData(colours, 24, Genesis::VBO_COLOUR);
}

void VertexBuffer::CopyPositions(const PositionData& data)
{
    CopyData(&data[0][0], data.size() * 3, VBO_POSITION);
}

void VertexBuffer::CopyPositions(const PositionData& data, size_t count)
{
    CopyData(&data[0][0], count * 3, VBO_POSITION);
}

void VertexBuffer::CopyUVs(const UVData& data)
{
    CopyData(&data[0][0], data.size() * 2, VBO_UV);
}

void VertexBuffer::CopyUVs(const UVData& data, size_t count)
{
    CopyData(&data[0][0], count * 2, VBO_UV);
}

void VertexBuffer::CopyNormals(const NormalData& data)
{
    SDL_assert(m_Flags & VBO_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, m_Normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.size(), data.data(), GL_DYNAMIC_DRAW);
}

void VertexBuffer::CopyTangents(const TangentData& data)
{
    SDL_assert(m_Flags & VBO_TANGENT);
    glBindBuffer(GL_ARRAY_BUFFER, m_Tangent);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.size(), data.data(), GL_DYNAMIC_DRAW);
}

void VertexBuffer::CopyBitangents(const BitangentData& data)
{
    SDL_assert(m_Flags & VBO_BITANGENT);
    glBindBuffer(GL_ARRAY_BUFFER, m_Bitangent);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.size(), data.data(), GL_DYNAMIC_DRAW);
}

void VertexBuffer::CopyColours(const ColourData& data)
{
    CopyData(&data[0][0], data.size() * 4, VBO_COLOUR);
}

void VertexBuffer::CopyColours(const ColourData& data, size_t count)
{
    CopyData(&data[0][0], count * 4, VBO_COLOUR);
}

void VertexBuffer::CopyIndices(const IndexData& data)
{
    SDL_assert(m_Flags & VBO_INDEX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32_t), data.data(), GL_DYNAMIC_DRAW);
    m_Size[GetSizeIndex(VBO_INDEX)] = data.size();
}

void VertexBuffer::CopyData(const float* pData, size_t size, unsigned int destination)
{
    size *= sizeof(float);

    if (destination == VBO_POSITION)
    {
        SDL_assert(m_Flags & VBO_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, m_Position);
    }
    else if (destination == VBO_UV)
    {
        SDL_assert(m_Flags & VBO_UV);
        glBindBuffer(GL_ARRAY_BUFFER, m_UV);
    }
    else if (destination == VBO_COLOUR)
    {
        SDL_assert(m_Flags & VBO_COLOUR);
        glBindBuffer(GL_ARRAY_BUFFER, m_Colour);
    }
    else
    {
        SDL_assert(false); // Not implemented.
    }

    const unsigned int idx = GetSizeIndex(destination);
    if (size <= m_Size[idx])
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, pData);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, size, pData, GL_DYNAMIC_DRAW);
        m_Size[idx] = size;
    }
}

void VertexBuffer::Draw(size_t numVertices /* = 0 */)
{
    Draw(0, numVertices);
}

void VertexBuffer::Draw(size_t startVertex, size_t numVertices)
{
    glBindVertexArray(m_VAO);

    size_t maxVertices = 0;
    if (m_Flags & VBO_INDEX)
    {
        SDL_assert(numVertices == 0);
        maxVertices = m_Size[GetSizeIndex(VBO_INDEX)];
    }
    else
    {
        maxVertices = m_Size[GetSizeIndex(VBO_POSITION)] / ((m_Flags & VB_2D) ? 2 : 3) / sizeof(float);
        SDL_assert(startVertex + numVertices <= maxVertices);
    }
    SDL_assert(maxVertices > 0);

    if (numVertices == 0)
    {
        numVertices = maxVertices;
    }

    static const int sAttribIndexPosition = 0;
    static const int sAttribIndexUV = 1;
    static const int sAttribIndexNormal = 2;
    static const int sAttribIndexColor = 3;
    static const int sAttribIndexTangent = 4;
    static const int sAttribIndexBitangent = 5;

    if (m_Flags & VBO_POSITION)
    {
        glEnableVertexAttribArray(sAttribIndexPosition);
        glBindBuffer(GL_ARRAY_BUFFER, m_Position);
        glVertexAttribPointer(sAttribIndexPosition, (m_Flags & VB_2D) ? 2 : 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    if (m_Flags & VBO_UV)
    {
        glEnableVertexAttribArray(sAttribIndexUV);
        glBindBuffer(GL_ARRAY_BUFFER, m_UV);
        glVertexAttribPointer(sAttribIndexUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    if (m_Flags & VBO_NORMAL)
    {
        glEnableVertexAttribArray(sAttribIndexNormal);
        glBindBuffer(GL_ARRAY_BUFFER, m_Normal);
        glVertexAttribPointer(sAttribIndexNormal, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    }

    if (m_Flags & VBO_COLOUR)
    {
        glEnableVertexAttribArray(sAttribIndexColor);
        glBindBuffer(GL_ARRAY_BUFFER, m_Colour);
        glVertexAttribPointer(sAttribIndexColor, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    if (m_Flags & VBO_TANGENT)
    {
        glEnableVertexAttribArray(sAttribIndexTangent);
        glBindBuffer(GL_ARRAY_BUFFER, m_Tangent);
        glVertexAttribPointer(sAttribIndexTangent, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    }

    if (m_Flags & VBO_BITANGENT)
    {
        glEnableVertexAttribArray(sAttribIndexBitangent);
        glBindBuffer(GL_ARRAY_BUFFER, m_Bitangent);
        glVertexAttribPointer(sAttribIndexBitangent, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    }

    if (m_Flags & VBO_INDEX)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Index);
        glDrawElements(m_Mode, static_cast<GLsizei>(numVertices), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(m_Mode, static_cast<GLint>(startVertex), static_cast<GLsizei>(numVertices));
    }

    if (m_Flags & VBO_POSITION)
    {
        glDisableVertexAttribArray(sAttribIndexPosition);
    }

    if (m_Flags & VBO_UV)
    {
        glDisableVertexAttribArray(sAttribIndexUV);
    }

    if (m_Flags & VBO_NORMAL)
    {
        glDisableVertexAttribArray(sAttribIndexNormal);
    }

    if (m_Flags & VBO_COLOUR)
    {
        glDisableVertexAttribArray(sAttribIndexColor);
    }

    if (m_Flags & VBO_TANGENT)
    {
        glDisableVertexAttribArray(sAttribIndexTangent);
    }

    if (m_Flags & VBO_BITANGENT)
    {
        glDisableVertexAttribArray(sAttribIndexBitangent);
    }

    FrameWork::GetRenderSystem()->IncreaseDrawCallCount();
}
} // namespace Genesis
