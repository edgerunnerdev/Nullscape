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

#include "resourcemodel.h"

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/string.h>
#include <externalheadersend.hpp>
// clang-format on

#include "resources/resourceshader.hpp"
#include "../genesis.h"
#include "../rendersystem.h"
#include "../shaderuniform.h"
#include "render/debugrender.h"
#include "modelserialization.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <log.hpp>
#include <sstream>
#include <stdio.h>

namespace Genesis
{

///////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////

Mesh::Mesh(const Serialization::Mesh* pMesh)
    : m_NumVertices(pMesh->header.vertices)
    , m_NumUVs(pMesh->uvChannels.empty() ? 0 : pMesh->uvChannels[0].uvs.size())
    , m_NumTriangles(pMesh->header.triangles)
    , m_MaterialIndex(pMesh->header.materialIndex)
{
    unsigned int vboFlags = VBO_POSITION | VBO_INDEX;

    IndexData indices;
    indices.reserve(m_NumTriangles * 3);
    for (uint32_t i = 0; i < m_NumTriangles; i++)
    {
        indices.push_back(pMesh->triangles[i].v1);
        indices.push_back(pMesh->triangles[i].v2);
        indices.push_back(pMesh->triangles[i].v3);
    }

    PositionData positions;
    positions.reserve(m_NumVertices);
    for (uint32_t i = 0; i < m_NumVertices; i++)
    {
        positions.emplace_back(pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z);
        m_DebugPositions.emplace_back(pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z);
    }

    UVData uvs;
    if (!pMesh->uvChannels.empty())
    {
        vboFlags |= VBO_UV;
        uvs.reserve(m_NumVertices);
        for (uint32_t i = 0; i < m_NumVertices; i++)
        {
            uvs.emplace_back(pMesh->uvChannels[0].uvs[i].u, pMesh->uvChannels[0].uvs[i].v);
        }
    }

    NormalData normals;
    if (!pMesh->normals.empty())
    {
        vboFlags |= VBO_NORMAL;
        normals.reserve(m_NumVertices);
        for (uint32_t i = 0; i < m_NumVertices; i++)
        {
            normals.emplace_back(pMesh->normals[i].x, pMesh->normals[i].y, pMesh->normals[i].z);
            m_DebugNormals.emplace_back(pMesh->normals[i].x, pMesh->normals[i].y, pMesh->normals[i].z);
        }
    }

    TangentData tangents;
    if (!pMesh->tangents.empty())
    {
        vboFlags |= VBO_TANGENT;
        tangents.reserve(m_NumVertices);
        for (uint32_t i = 0; i < m_NumVertices; i++)
        {
            tangents.emplace_back(pMesh->tangents[i].x, pMesh->tangents[i].y, pMesh->tangents[i].z);
            m_DebugTangents.emplace_back(pMesh->tangents[i].x, pMesh->tangents[i].y, pMesh->tangents[i].z);
        }
    }

    BitangentData bitangents;
    if (!pMesh->bitangents.empty())
    {
        vboFlags |= VBO_BITANGENT;
        bitangents.reserve(m_NumVertices);
        for (uint32_t i = 0; i < m_NumVertices; i++)
        {
            bitangents.emplace_back(pMesh->bitangents[i].x, pMesh->bitangents[i].y, pMesh->bitangents[i].z);
            m_DebugBitangents.emplace_back(pMesh->bitangents[i].x, pMesh->bitangents[i].y, pMesh->bitangents[i].z);
        }
    }

    m_pVertexBuffer = std::make_shared<VertexBuffer>(GeometryType::Triangle, vboFlags);
    if (vboFlags & VBO_POSITION)
    {
        m_pVertexBuffer->CopyPositions(positions);
    }
    if (vboFlags & VBO_UV)
    {
        m_pVertexBuffer->CopyUVs(uvs);
    }
    if (vboFlags & VBO_NORMAL)
    {
        m_pVertexBuffer->CopyNormals(normals);
    }
    if (vboFlags & VBO_TANGENT)
    {
        m_pVertexBuffer->CopyTangents(tangents);
    }
    if (vboFlags & VBO_BITANGENT)
    {
        m_pVertexBuffer->CopyBitangents(bitangents);
    }
    if (vboFlags & VBO_INDEX)
    {
        m_pVertexBuffer->CopyIndices(indices);
    }
}

Mesh::~Mesh()
{

}

void Mesh::Render(const glm::mat4& modelTransform, const Materials& materials)
{
    Material* pMaterial = materials[m_MaterialIndex].get();
    pMaterial->GetShader()->Use(modelTransform, &pMaterial->GetShaderUniformInstances());
    m_pVertexBuffer->Draw();
}

void Mesh::DebugRender(Render::DebugRender* pDebugRender, ResourceModel::DebugRenderFlags flags) 
{
    if (flags != ResourceModel::DebugRenderFlags::None)
    {
        const size_t c = m_DebugPositions.size();

        if (flags & ResourceModel::DebugRenderFlags::Normals)
        {
            for (size_t i = 0; i < c; ++i)
            {
                pDebugRender->DrawLine(m_DebugPositions[i], m_DebugPositions[i] + m_DebugNormals[i], glm::vec3(1, 0, 0));
            }
        }
        
        if (flags & ResourceModel::DebugRenderFlags::Tangents)
        {
            for (size_t i = 0; i < c; ++i)
            {
                pDebugRender->DrawLine(m_DebugPositions[i], m_DebugPositions[i] + m_DebugTangents[i], glm::vec3(0, 1, 0));
            }
        }
        
        if (flags & ResourceModel::DebugRenderFlags::Bitangents)
        {
            for (size_t i = 0; i < c; ++i)
            {
                pDebugRender->DrawLine(m_DebugPositions[i], m_DebugPositions[i] + m_DebugBitangents[i], glm::vec3(0, 0, 1));
            }
        }
    }
}

void Mesh::Render(const glm::mat4& modelTransform, Material* pOverrideMaterial)
{
    pOverrideMaterial->GetShader()->Use(modelTransform, &pOverrideMaterial->GetShaderUniformInstances());
    m_pVertexBuffer->Draw();
}

size_t Mesh::GetVertexCount() const 
{
    return static_cast<size_t>(m_NumVertices);
}

size_t Mesh::GetTriangleCount() const 
{
    return static_cast<size_t>(m_NumTriangles);
}

///////////////////////////////////////////////////////
// ResourceModel
///////////////////////////////////////////////////////

ResourceModel::ResourceModel(const Filename& filename)
    : ResourceGeneric(filename)
    , m_DebugRenderFlags(DebugRenderFlags::None)
{
}

ResourceModel::~ResourceModel()
{

}

bool ResourceModel::Load()
{
    std::ifstream file(GetFilename().GetFullPath(), std::ios::in | std::ios::binary);
    if (file.good() == false)
    {
        Log::Error() << "Couldn't load " << GetFilename().GetFullPath();
        return false;
    }

    Serialization::Model model;
    auto state = bitsery::quickDeserialization<bitsery::InputStreamAdapter>(file, model);
    file.close();
    if (state.first != bitsery::ReaderError::NoError || state.second != true)
    {
        Log::Error() << "Failed to deserialize " << GetFilename().GetFullPath();
        return false;
    }

    if (ReadHeader(&model) == false)
    {
        Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read header.";
        return false;
    }
    else if (ReadMaterials(&model) == false)
    {
        Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read materials.";
        return false;
    }
    else if (ReadMeshes(&model) == false)
    {
        Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read meshes.";
        return false;
    }
    else if (ReadPhysicsMesh(&model) == false)
    {
        Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to load physics mesh.";
        return false;
    }
    else
    {
        Log::Info() << "Loaded model " << GetFilename().GetFullPath() << ": " << static_cast<int>(model.header.meshes) << " meshes, " << static_cast<int>(model.header.materials) << " materials.";
        m_State = ResourceState::Loaded;
        return true;
    }
}

bool ResourceModel::ReadHeader(const Serialization::Model* pModel)
{
    if (pModel->header.format != "GMDL")
    {
        Log::Error() << "Header error: not a GMDL file.";
        return false;
    }
    else
    {
        return true;
    }
}

bool ResourceModel::ReadMaterials(const Serialization::Model* pModel)
{
    for (uint8_t i = 0; i < pModel->header.materials; ++i)
    {
        const Serialization::ModelMaterial& serializationMaterial = pModel->materials[i];
        MaterialSharedPtr pMaterial = std::make_shared<Material>();
        pMaterial->SetName(serializationMaterial.name);
        const std::string shaderName = serializationMaterial.shader;

        std::stringstream shaderPath;
        shaderPath << "data/shaders/" << shaderName << ".glsl";
        ResourceShader* pShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>(shaderPath.str());
        if (pShader == nullptr)
        {
            Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": invalid shader " << shaderName;
            return false;
        }
        else
        {
            pMaterial->SetShader(pShader);
        }

        int textureMap = 0;
        for (auto& binding : serializationMaterial.bindings)
        {
            const std::string& name = binding.first;
            const std::string filename = GetFilename().GetDirectory() + binding.second;

            ShaderUniformSharedPtr pShaderUniform = pShader->RegisterUniform(name.c_str(), ShaderUniformType::Texture);
            if (pShaderUniform == nullptr)
            {
                Log::Error() << "Model '" << GetFilename().GetFullPath() << "', shader '" << shaderName << "', couldn't find texture uniform named '" << name << "'.";
                return false;
            }

            ResourceImage* pResourceImage = FrameWork::GetResourceManager()->GetResource<ResourceImage*>(filename);
            if (pResourceImage == nullptr)
            {
                Log::Error() << "Unable to load " << filename;
                return false;
            }
            else
            {
                ShaderUniformInstance instance(pShaderUniform);
                instance.Set(pResourceImage, GL_TEXTURE0 + textureMap);
                pMaterial->GetShaderUniformInstances().push_back(std::move(instance));
                pMaterial->GetResourceImages().push_back(pResourceImage);
                textureMap++;
            }
        }

        m_Materials.push_back(std::move(pMaterial));
    }
    return true;
}

bool ResourceModel::ReadMeshes(const Serialization::Model* pModel) 
{
    for (const Serialization::Mesh& serializationMesh : pModel->meshes)
    {
        MeshUniquePtr pMesh = std::make_unique<Mesh>(&serializationMesh);
        m_Meshes.push_back(std::move(pMesh));
    }
    return true;
}

bool ResourceModel::ReadPhysicsMesh(const Serialization::Model* pModel)
{
    if (pModel->header.hasPhysicsMesh)
    {
        m_pPhysicsMesh = std::make_unique<Mesh>(&pModel->physicsMesh);
    }
    return true;
}

bool ResourceModel::GetDummy(const std::string& name, glm::vec3* pPosition) const
{
    // No longer used, use an entity component instead.
    return false;
}

// You can optionally pass an override material to be used instead of the normal materials this model would use.
void ResourceModel::Render(const glm::mat4& modelTransform, Material* pOverrideMaterial /* = nullptr */)
{
    if (pOverrideMaterial == nullptr)
    {
        for (auto& pMesh : m_Meshes)
        {
            pMesh->Render(modelTransform, GetMaterials());
        }
    }
    else
    {
        for (auto& pMesh : m_Meshes)
        {
            pMesh->Render(modelTransform, pOverrideMaterial);
        }
    }

    if (m_pPhysicsMesh && (m_DebugRenderFlags & DebugRenderFlags::PhysicsMesh))
    {
        m_pPhysicsMesh->Render(modelTransform, nullptr);
    }
}

void ResourceModel::DebugRender(Render::DebugRender* pDebugRender, DebugRenderFlags flags) 
{
    for (auto& pMesh : m_Meshes)
    {
        pMesh->DebugRender(pDebugRender, flags);
    }
}

size_t ResourceModel::GetVertexCount() const
{
    size_t count = 0;
    for (auto& pMesh : m_Meshes)
    {
        count += pMesh->GetVertexCount();
    }
    return count;
}

size_t ResourceModel::GetTriangleCount() const
{
    size_t count = 0;
    for (auto& pMesh : m_Meshes)
    {
        count += pMesh->GetTriangleCount();
    }
    return count;
}

} // namespace Genesis