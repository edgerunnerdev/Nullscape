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

#include "coredefines.h"
#include "glm/gtx/transform.hpp"
#include "rendersystem.fwd.h"

#include <array>
#include <vector>

namespace Genesis
{

///////////////////////////////////////////////////////////////////////////////
// VertexBuffer
///////////////////////////////////////////////////////////////////////////////

static const unsigned int VBO_POSITION = 1;
static const unsigned int VBO_UV = 1 << 1;
static const unsigned int VBO_NORMAL = 1 << 2;
static const unsigned int VBO_COLOUR = 1 << 3;
static const unsigned int VBO_INDEX = 1 << 4;
static const unsigned int VB_2D = 1 << 5;

using PositionData = std::vector<glm::vec3>;
using UVData = std::vector<glm::vec2>;
using NormalData = std::vector<glm::vec3>;
using ColourData = std::vector<glm::vec4>;
using IndexData = std::vector<uint32_t>;

enum class GeometryType
{
    Triangle,
    Line,
    LineStrip
};

class VertexBuffer
{
public:
    VertexBuffer();
    VertexBuffer(GeometryType type, unsigned int flags);
    ~VertexBuffer();

    void CopyPositions(const PositionData& data);
    void CopyPositions(const PositionData& data, size_t count);
    void CopyUVs(const UVData& data);
    void CopyUVs(const UVData& data, size_t count);
    void CopyNormals(const NormalData& data);
    void CopyNormals(const NormalData& data, size_t count);
    void CopyColours(const ColourData& data);
    void CopyColours(const ColourData& data, size_t count);
    void CopyIndices(const IndexData& data);
    void CopyData(const float* pData, size_t count, unsigned int destination);

    void Draw(size_t numVertices = 0); // Draw the vertex buffer. Passing 0 to this function will draw the entire buffer.
    void Draw(size_t startVertex, size_t numVertices);

    void CreateUntexturedQuad(float x, float y, float width, float height);
    void CreateUntexturedQuad(float x, float y, float width, float height, const glm::vec4& colour);
    void CreateTexturedQuad(float x, float y, float width, float height);
    void CreateTexturedQuad(float x, float y, float width, float height, const glm::vec4& colour);

private:
    void SetModeFromGeometryType(GeometryType type);
    unsigned int GetSizeIndex(unsigned int flag) const;

    unsigned int m_Flags;
    GLuint m_VAO;
    GLuint m_Position;
    GLuint m_UV;
    GLuint m_Normal;
    GLuint m_Colour;
    GLuint m_Index;
    std::array<size_t, 5> m_Size;
    GLenum m_Mode;
};
GENESIS_DECLARE_SMART_PTR(VertexBuffer);

// Translates one of the VBO_ flags into an actual size index which we can use.
// log( flag ) / log( 2 ) is the same as log2( flag ) but log2 isn't available in this version of VS for some reason.
inline unsigned int VertexBuffer::GetSizeIndex(unsigned int flag) const
{
    return (unsigned int)(log(flag) / log(2));
}
} // namespace Genesis
