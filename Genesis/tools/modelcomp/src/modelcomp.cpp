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

#include "modelcomp.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <fstream>
#include <log.hpp>
#include "material.hpp"

namespace Genesis
{
namespace ResComp
{

ModelComp::ModelComp() {}
ModelComp::~ModelComp() {}

int ModelComp::Run()
{
    if (ReadAsset(GetFile()) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Invalid model asset file.");
        return -1;
    }
    
    if (std::filesystem::exists(m_SourceModelPath) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Model file doesn't exist.");
        return -1;
    }

    Assimp::Importer importer;
    const int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals | aiProcess_SortByPType |
                      aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
    const aiScene* pScene = importer.ReadFile(m_SourceModelPath.generic_string(), flags);
    if (pScene == nullptr)
    {
        OnAssetCompilationFailed(GetFile(), importer.GetErrorString());
        return -1;
    }

    if (ValidateMaterials(pScene) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Material validation failed.");
        return -1;
    }

    std::filesystem::path targetModelPath = GetTargetModelPath(m_SourceModelPath);
    if (Compile(pScene, targetModelPath))
    {
        OnResourceBuilt(GetFile(), m_SourceModelPath, targetModelPath);
        return 0;
    }
    else
    {
        OnAssetCompilationFailed(GetFile(), "Failed to compile file.");
        return -1;
    }
}

bool ModelComp::ReadAsset(const std::filesystem::path& assetPath) 
{
    using namespace nlohmann;
    std::ifstream file(assetPath);
    if (file.good())
    {
        json j;
        file >> j;
        file.close();

        json::iterator it = j.find("source");
        if (it == j.end() || it->is_string() == false)
        {
            Core::Log::Error() << "Couldn't find required 'source' field in asset " << assetPath;
            return false;
        }
        else
        {
            m_SourceModelPath = assetPath;
            m_SourceModelPath = m_SourceModelPath.remove_filename() / it->get<std::string>();
        }

        it = j.find("materials");
        if (it == j.end() || it->is_array() == false)
        {
            Core::Log::Error() << "Couldn't find required 'materials' array in asset " << assetPath;
            return false;
        }
        else
        {
            for (json& jMaterial : *it)
            {
                MaterialUniquePtr pMaterial = std::make_unique<Material>();

                json::iterator materialIt = jMaterial.find("name");
                if (materialIt == jMaterial.end() || materialIt->is_string() == false)
                {
                    Core::Log::Error() << "Couldn't find material name in asset " << assetPath;
                    return false;
                }
                else
                {
                    pMaterial->SetName(materialIt->get<std::string>());
                }

                materialIt = jMaterial.find("shader");
                if (materialIt == jMaterial.end() || materialIt->is_string() == false)
                {
                    Core::Log::Error() << "Couldn't find required 'shader' string for material " << pMaterial->GetName() << " in asset " << assetPath;
                    return false;
                }
                else
                {
                    pMaterial->SetShader(materialIt->get<std::string>());
                }

                materialIt = jMaterial.find("bindings");
                if (materialIt == jMaterial.end() || materialIt->is_array() == false)
                {
                    Core::Log::Error() << "Couldn't find required 'bindings' array for material " << pMaterial->GetName() << " in asset " << assetPath;
                    return false;
                }
                else
                {
                    Material::Bindings& bindings = pMaterial->GetBindings();
                    for (json& jBinding : *materialIt)
                    {
                        json::iterator bindingIt = jBinding.find("name");
                        if (bindingIt == jBinding.end() || bindingIt->is_string() == false)
                        {
                            Core::Log::Error() << "Couldn't find required 'name' string in bindings for material " << pMaterial->GetName() << " in asset " << assetPath;
                            return false;
                        }
                        std::string bindingName = bindingIt->get<std::string>();

                        bindingIt = jBinding.find("filename");
                        if (bindingIt == jBinding.end() || bindingIt->is_string() == false)
                        {
                            Core::Log::Error() << "Couldn't find required 'filename' string in bindings for material " << pMaterial->GetName() << " in asset " << assetPath;
                            return false;
                        }
                        std::string bindingFilename = bindingIt->get<std::string>();

                        bindings[bindingName] = bindingFilename;
                    }
                }
                
                m_Materials[pMaterial->GetName()] = std::move(pMaterial);
            }
        }

        return true;
    }
    return false;
}

bool ModelComp::ValidateMaterials(const aiScene* pScene) 
{
    bool mismatch = false;
    if (m_Materials.size() != pScene->mNumMaterials)
    {
        Core::Log::Error() << "Different number of materials between model file and asset.";
        mismatch = true;
    }

    for (auto& pMaterial : m_Materials)
    {
        bool found = false;
        for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
        {
            if (pMaterial.first == pScene->mMaterials[i]->GetName().C_Str())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mismatch = true;
            break;
        }
    }

    if (mismatch)
    {
        Core::Log::Error() << "Material mismatch between asset and model file.";

        Core::Log::Error() << "Asset:";
        for (auto& pMaterial : m_Materials)
        {
            Core::Log::Error() << "- " << pMaterial.first;
        }

        Core::Log::Error() << "Model file:";
        for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
        {
            Core::Log::Error() << "- " << pScene->mMaterials[i]->GetName().C_Str();
        }
    }

    return !mismatch;
}

std::filesystem::path ModelComp::GetTargetModelPath(const std::filesystem::path& sourceModelPath) const
{
    std::filesystem::path targetPath = GetDataDir() / std::filesystem::relative(GetFile(), GetAssetsDir()).remove_filename();
    std::filesystem::create_directories(targetPath);
    const std::string targetFileName = sourceModelPath.stem().string() + ".gmdl";
    return targetPath / targetFileName;
}

bool ModelComp::Compile(const aiScene* pScene, std::filesystem::path& targetModelPath)
{
    std::ofstream file(targetModelPath, std::ios::out | std::ios::trunc);
    if (file.good())
    {
        WriteHeader(file, pScene);
        WriteMaterials(file, pScene);
        WriteMeshes(file, pScene);
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

void ModelComp::WriteHeader(std::ofstream& file, const aiScene* pScene)
{
    file << "GMDL";
    file << static_cast<uint8_t>(1); // Version
    file << static_cast<uint8_t>(pScene->mNumMaterials);
    file << static_cast<uint8_t>(pScene->mNumMeshes);
}

void ModelComp::WriteMaterials(std::ofstream& file, const aiScene* pScene) 
{
    for (auto& materialPair : m_Materials)
    {
        Material* pMaterial = materialPair.second.get();
        file << pMaterial->GetName();
        file << pMaterial->GetShader();
        Material::Bindings& bindings = pMaterial->GetBindings();
        file << bindings.size();
        for (auto& binding : bindings)
        {
            file << binding.first;
            file << binding.second;
        }
    }
}

void ModelComp::WriteMeshes(std::ofstream& file, const aiScene* pScene)
{
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        const aiMesh* pMesh = pScene->mMeshes[i];
        WriteMeshHeader(file, pMesh);
        WriteMesh(file, pMesh);
    }
}

void ModelComp::WriteMeshHeader(std::ofstream& file, const aiMesh* pMesh)
{
    file << static_cast<uint8_t>(pMesh->mMaterialIndex);
    file << static_cast<uint32_t>(pMesh->mNumVertices);
    file << static_cast<uint32_t>(pMesh->mNumFaces);
    file << static_cast<uint32_t>(pMesh->GetNumUVChannels());
    // file << static_cast<uint32_t>(pMesh->mNumBones); // Not currently implemented.
}

void ModelComp::WriteMesh(std::ofstream& file, const aiMesh* pMesh)
{
    for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
    {
        const aiVector3D& vertex = pMesh->mVertices[i];
        file << vertex.x << vertex.y << vertex.z;
    }

    for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
    {
        const aiFace& face = pMesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            file << static_cast<uint32_t>(face.mIndices[j]);
        }
    }

    for (uint32_t i = 0; i < pMesh->GetNumUVChannels(); ++i)
    {
        for (uint32_t j = 0; j < pMesh->mNumVertices; ++j)
        {
            const aiVector3D& uvw = pMesh->mTextureCoords[i][j];
            file << uvw.x << uvw.y;
        }
    }

    for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
    {
        const aiVector3D& normal = pMesh->mNormals[i];
        file << normal.x << normal.y << normal.z;
    }
}

} // namespace ResComp
} // namespace Genesis
