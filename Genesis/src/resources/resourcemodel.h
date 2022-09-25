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
#include "resources/resourcegeneric.h"
#include "render/material.hpp"

#include <array>
#include <fstream>
#include <map>
#include <stdio.h>
#include <vector>

namespace Genesis
{

namespace Render
{
class DebugRender;
}

namespace Serialization
{
struct Mesh;
struct Model;
}

class Mesh;
class VertexBuffer;

static const short MODEL_VERSION = 1;

using Materials = std::vector<MaterialSharedPtr>;
using ResourceImages = std::vector<ResourceImage*>;

typedef std::map<std::string, glm::vec3> DummyMap;

GENESIS_DECLARE_SMART_PTR(Mesh);


///////////////////////////////////////////////////////
// ResourceModel
///////////////////////////////////////////////////////

class ResourceModel : public ResourceGeneric
{
public:
    enum DebugRenderFlags
    {
        None = 0,
        Normals = 1 << 0,
        Tangents = 1 << 1,
        Bitangents = 1 << 2,
        PhysicsMesh = 1 << 3
    };

    ResourceModel(const Filename& filename);
    virtual ~ResourceModel();
    virtual ResourceType GetType() const override;
    virtual bool Load() override;

    void Render(const glm::mat4& modelTransform, Material* pOverrideMaterial = nullptr);
    void DebugRender(Render::DebugRender* pDebugRender, DebugRenderFlags flags);
    bool GetDummy(const std::string& name, glm::vec3* pPosition) const;
    Materials& GetMaterials();

    size_t GetVertexCount() const;
    size_t GetTriangleCount() const;

private:
    bool ReadHeader(const Serialization::Model* pModel);
    bool ReadMaterials(const Serialization::Model* pModel);
    bool ReadMeshes(const Serialization::Model* pModel);

    using Meshes = std::vector<MeshUniquePtr>;

    Materials m_Materials;
    Meshes m_Meshes;
    MeshUniquePtr m_pPhysicsMesh;
    DebugRenderFlags m_DebugRenderFlags;
};

inline Materials& ResourceModel::GetMaterials()
{
    return m_Materials;
}

inline ResourceType ResourceModel::GetType() const
{
    return ResourceType::Texture;
}


///////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////

class Mesh
{
public:
    Mesh(const Serialization::Mesh* pMesh);
    ~Mesh();

    void Render(const glm::mat4& modelTransform, const Materials& materials);
    void Render(const glm::mat4& modelTransform, Material* pOverrideMaterial);
    void DebugRender(Render::DebugRender* pDebugRender, ResourceModel::DebugRenderFlags flags);

    size_t GetVertexCount() const;
    size_t GetTriangleCount() const;

private:
    uint32_t m_NumVertices;
    uint32_t m_NumUVs;
    uint32_t m_NumTriangles;
    uint32_t m_MaterialIndex;
    VertexBufferSharedPtr m_pVertexBuffer;

    std::vector<glm::vec3> m_DebugPositions;
    std::vector<glm::vec3> m_DebugNormals;
    std::vector<glm::vec3> m_DebugTangents;
    std::vector<glm::vec3> m_DebugBitangents;
};

} // namespace Genesis