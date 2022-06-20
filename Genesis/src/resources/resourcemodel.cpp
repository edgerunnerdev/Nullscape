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

#include "../genesis.h"
#include "../rendersystem.h"
#include "../shadercache.h"
#include "../shaderuniform.h"
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
    , m_NumUVs(pMesh->uvChannels[0].uvs.size())
    , m_NumTriangles(pMesh->header.faces)
    , m_pVertexBuffer(nullptr)
    , m_MaterialIndex(pMesh->header.materialIndex)
{
    #if 1
    size_t numVertices = m_NumTriangles * 3;

    m_VertexBufferPosData.reserve(numVertices);
    m_VertexBufferNormalData.reserve(numVertices);
    m_VertexBufferUvData.reserve(numVertices);

    IndexData indices;
    indices.reserve(m_NumTriangles);
    for (uint32_t i = 0; i < m_NumTriangles; i++)
    {
        indices.push_back(pMesh->faces[i].v1);
        indices.push_back(pMesh->faces[i].v2);
        indices.push_back(pMesh->faces[i].v3);
    }

    size_t offset = 0;
    for (uint32_t i = 0; i < m_NumTriangles; i++)
    {
        const Serialization::Vertex& v1 = pMesh->vertices[pMesh->faces[i].v1];
        m_VertexBufferPosData.push_back(glm::vec3(v1.x, v1.y, v1.z));
        const Serialization::Vertex& v2 = pMesh->vertices[pMesh->faces[i].v2];
        m_VertexBufferPosData.push_back(glm::vec3(v2.x, v2.y, v2.z));
        const Serialization::Vertex& v3 = pMesh->vertices[pMesh->faces[i].v3];
        m_VertexBufferPosData.push_back(glm::vec3(v3.x, v3.y, v3.z));

        const Serialization::Normal& n1 = pMesh->normals[pMesh->faces[i].v1];
        m_VertexBufferNormalData.push_back(glm::vec3(n1.x, n1.y, n1.z));
        const Serialization::Normal& n2 = pMesh->normals[pMesh->faces[i].v2];
        m_VertexBufferNormalData.push_back(glm::vec3(n2.x, n2.y, n2.z));
        const Serialization::Normal& n3 = pMesh->normals[pMesh->faces[i].v3];
        m_VertexBufferNormalData.push_back(glm::vec3(n3.x, n3.y, n3.z));

        const Serialization::UV& t1 = pMesh->uvChannels[0].uvs[pMesh->faces[i].v1];
        m_VertexBufferUvData.push_back(glm::vec2(t1.u, t1.v));
        const Serialization::UV& t2 = pMesh->uvChannels[0].uvs[pMesh->faces[i].v2];
        m_VertexBufferUvData.push_back(glm::vec2(t2.u, t2.v));
        const Serialization::UV& t3 = pMesh->uvChannels[0].uvs[pMesh->faces[i].v3];
        m_VertexBufferUvData.push_back(glm::vec2(t3.u, t3.v));
    }

    m_pVertexBuffer = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_UV | VBO_NORMAL);
    m_pVertexBuffer->CopyPositions(m_VertexBufferPosData);
    m_pVertexBuffer->CopyNormals(m_VertexBufferNormalData);
    m_pVertexBuffer->CopyUVs(m_VertexBufferUvData);

    #else

    m_VertexBufferPosData.reserve(m_NumVertices);
    m_VertexBufferNormalData.reserve(m_NumVertices);
    m_VertexBufferUvData.reserve(m_NumVertices);

    IndexData indices;
    indices.reserve(m_NumTriangles);
    for (uint32_t i = 0; i < m_NumTriangles; i++)
    {
        indices.push_back(pMesh->faces[i].v1);
        indices.push_back(pMesh->faces[i].v2);
        indices.push_back(pMesh->faces[i].v3);
    }

    for (uint32_t i = 0; i < m_NumVertices; i++)
    {
        m_VertexBufferPosData.push_back(glm::vec3(pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z));
        m_VertexBufferNormalData.push_back(glm::vec3(pMesh->normals[i].x, pMesh->normals[i].y, pMesh->normals[i].z));
        m_VertexBufferUvData.push_back(glm::vec2(pMesh->uvChannels[0].uvs[i].u, pMesh->uvChannels[0].uvs[i].v));
    }

    m_pVertexBuffer = new VertexBuffer(GeometryType::Triangle, VBO_POSITION | VBO_UV | VBO_NORMAL | VBO_INDEX);
    m_pVertexBuffer->CopyPositions(m_VertexBufferPosData);
    m_pVertexBuffer->CopyNormals(m_VertexBufferNormalData);
    m_pVertexBuffer->CopyUVs(m_VertexBufferUvData);
    m_pVertexBuffer->CopyIndices(indices);

    #endif

}

Mesh::~Mesh()
{
    delete m_pVertexBuffer;
}

void Mesh::Render(const glm::mat4& modelTransform, const Materials& materials)
{
    Material* pMaterial = materials[m_MaterialIndex].get();
    pMaterial->GetShader()->Use(modelTransform, &pMaterial->GetShaderUniformInstances());
    m_pVertexBuffer->Draw();
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
        Core::Log::Error() << "Couldn't load " << GetFilename().GetFullPath();
        return false;
    }

    Serialization::Model model;
    auto state = bitsery::quickDeserialization<bitsery::InputStreamAdapter>(file, model);
    file.close();
    if (state.first != bitsery::ReaderError::NoError || state.second != true)
    {
        Core::Log::Error() << "Failed to deserialize " << GetFilename().GetFullPath();
        return false;
    }

    if (ReadHeader(&model) == false)
    {
        Core::Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read header.";
        return false;
    }
    else if (ReadMaterials(&model) == false)
    {
        Core::Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read materials.";
        return false;
    }
    else if (ReadMeshes(&model) == false)
    {
        Core::Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": Failed to read meshes.";
        return false;
    }
    else
    {
        Core::Log::Info() << "Loaded model " << GetFilename().GetFullPath() << ": " << model.header.meshes << " meshes, " << model.header.materials << " materials.";
        m_State = ResourceState::Loaded;
        return true;
    }
}

bool ResourceModel::ReadHeader(const Serialization::Model* pModel)
{
    if (pModel->header.format != "GMDL")
    {
        Core::Log::Error() << "Header error: not a GMDL file.";
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
        Shader* pShader = FrameWork::GetRenderSystem()->GetShaderCache()->Load(shaderName);
        if (pShader == nullptr)
        {
            Core::Log::Error() << "Couldn't load " << GetFilename().GetFullPath() << ": invalid shader " << shaderName;
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

            ShaderUniform* pShaderUniform = pShader->RegisterUniform(name.c_str(), ShaderUniformType::Texture);
            if (pShaderUniform == nullptr)
            {
                Core::Log::Error() << "Model '" << GetFilename().GetFullPath() << "', shader '" << shaderName << "', couldn't find texture uniform named '" << name << "'.";
                return false;
            }

            ResourceImage* pResourceImage = FrameWork::GetResourceManager()->GetResource<ResourceImage*>(filename);
            if (pResourceImage == nullptr)
            {
                Core::Log::Error() << "Unable to load " << filename;
                return false;
            }
            else
            {
                ShaderUniformInstance instance(pShaderUniform);
                instance.Set(pResourceImage, GL_TEXTURE0 + textureMap);
                pMaterial->GetShaderUniformInstances().push_back(instance);
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
        m_Meshes.emplace_back(&serializationMesh);
    }
    return true;
}

bool ResourceModel::GetDummy(const std::string& name, glm::vec3* pPosition) const
{
    std::string nameLowerCase(name);
    std::transform(nameLowerCase.begin(), nameLowerCase.end(), nameLowerCase.begin(),
                   [](char c) -> char
                   {
                       return static_cast<char>(std::tolower(c));
                   });

    DummyMap::const_iterator it = mDummyMap.find(nameLowerCase);
    if (it == mDummyMap.end())
    {
        return false;
    }
    else
    {
        *pPosition = it->second;
        return true;
    }
}

void ResourceModel::AddTMFDummy(FILE* fp)
{
    int len;
    fread(&len, sizeof(uint32_t), 1, fp);
    char* pBuffer = new char[len];
    fread(pBuffer, sizeof(char), len, fp);

    glm::vec3 pos;
    fread(&pos.x, sizeof(float), 1, fp);
    fread(&pos.y, sizeof(float), 1, fp);
    fread(&pos.z, sizeof(float), 1, fp);

    std::string dummyName(pBuffer);
    std::transform(dummyName.begin(), dummyName.end(), dummyName.begin(),
                   [](char c) -> char
                   {
                       return static_cast<char>(std::tolower(c));
                   });

    mDummyMap[dummyName] = pos;

    delete[] pBuffer;
}

void ResourceModel::AddTMFObject(FILE* fp)
{

}

// You can optionally pass an override material to be used instead of the normal materials this model would use.
void ResourceModel::Render(const glm::mat4& modelTransform, Material* pOverrideMaterial /* = nullptr */)
{
    if (pOverrideMaterial == nullptr)
    {
        for (auto& mesh : m_Meshes)
        {
            mesh.Render(modelTransform, GetMaterials());
        }
    }
    else
    {
        for (auto& mesh : m_Meshes)
        {
            mesh.Render(modelTransform, pOverrideMaterial);
        }
    }
}

size_t ResourceModel::GetVertexCount() const
{
    size_t count = 0;
    for (auto& mesh : m_Meshes)
    {
        count += mesh.GetVertexCount();
    }
    return count;
}

size_t ResourceModel::GetTriangleCount() const
{
    size_t count = 0;
    for (auto& mesh : m_Meshes)
    {
        count += mesh.GetTriangleCount();
    }
    return count;
}

} // namespace Genesis