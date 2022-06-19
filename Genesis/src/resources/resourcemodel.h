// Copyright 2014 Pedro Nunes
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

#include "rendersystem.h"
#include "resourcemanager.h"
#include "shaderuniforminstance.h"
#include "vertexbuffer.h"
#include "resources/resourceimage.h"
#include "render/material.hpp"

#include <array>
#include <fstream>
#include <map>
#include <stdio.h>
#include <vector>

namespace Genesis
{
class Mesh;
class TMFObject;
class VertexBuffer;

static const short MODEL_VERSION = 1;

using Materials = std::vector<MaterialSharedPtr>;
using ResourceImages = std::vector<ResourceImage*>;

typedef std::map<std::string, glm::vec3> DummyMap;
typedef std::vector<TMFObject*> TMFObjectList;

///////////////////////////////////////////////////////
// TMFObject
///////////////////////////////////////////////////////

class TMFObject
{
public:
    TMFObject();
    ~TMFObject();

    void Preload(FILE* fp);
    void Load();
    void Render(const glm::mat4& modelTransform, const Materials& materials);
    void Render(const glm::mat4& modelTransform, Material* pOverrideMaterial);

private:
    struct Index3
    {
        uint32_t v1, v2, v3;
    };

    struct Triangle
    {
        Index3 vertex;
        Index3 uv;
        glm::vec3 normal[3];
    };

    bool Serialise(FILE* fp);
    void BuildVertexBufferData();

    typedef std::vector<glm::vec3> VertexList;
    typedef std::vector<glm::vec2> UVList;
    typedef std::vector<Triangle> TriangleList;

    Uint32 m_NumVertices;
    Uint32 m_NumUVs;
    Uint32 m_NumTriangles;

    // Serialisation data (temporary, gets released after VBOs are created)
    VertexList m_VertexList;
    UVList m_UvList;
    TriangleList m_TriangleList;

    PositionData m_VertexBufferPosData;
    NormalData m_VertexBufferNormalData;
    UVData m_VertexBufferUvData;

    VertexBuffer* m_pVertexBuffer;

    unsigned int m_MaterialIndex;
};

///////////////////////////////////////////////////////
// ResourceModel
///////////////////////////////////////////////////////

class ResourceModel : public ResourceGeneric
{
public:
    ResourceModel(const Filename& filename);
    virtual ~ResourceModel();
    virtual ResourceType GetType() const override;
    virtual bool Load() override;

    void Render(const glm::mat4& modelTransform, Material* pOverrideMaterial = nullptr);
    bool GetDummy(const std::string& name, glm::vec3* pPosition) const;
    Materials& GetMaterials();
    void SetFlipAxis(bool value);

private:
    void AddTMFDummy(FILE* fp);
    void AddTMFObject(FILE* fp);
    void LoadMaterialLibrary(const std::string& filename);

    bool ReadHeader(std::ifstream& file);
    bool ReadMaterials(std::ifstream& file);

    std::string Read(std::ifstream& file, size_t count);

    Materials m_Materials;
    TMFObjectList mObjectList;
    DummyMap mDummyMap;
    bool mFlipAxis;

    uint8_t m_NumMaterials;
    uint8_t m_NumMeshes;
};

inline Materials& ResourceModel::GetMaterials()
{
    return m_Materials;
}

inline void ResourceModel::SetFlipAxis(bool value)
{
    mFlipAxis = value;
}

inline ResourceType ResourceModel::GetType() const
{
    return ResourceType::Texture;
}
} // namespace Genesis