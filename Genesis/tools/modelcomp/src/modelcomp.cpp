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

namespace Genesis
{
namespace ResComp
{

ModelComp::ModelComp() {}
ModelComp::~ModelComp() {}

int ModelComp::Run()
{
    std::filesystem::path sourceModelPath;
    if (GetSourceModelPath(GetFile(), sourceModelPath) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Invalid source model path.");
        return -1;
    }
    else if (std::filesystem::exists(sourceModelPath) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Model file doesn't exist.");
        return -1;
    }

    Assimp::Importer importer;
    const int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals | aiProcess_SortByPType |
                      aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
    const aiScene* pScene = importer.ReadFile(sourceModelPath.generic_string(), flags);
    if (pScene == nullptr)
    {
        OnAssetCompilationFailed(GetFile(), importer.GetErrorString());
        return -1;
    }

    std::filesystem::path targetModelPath;
    if (GetTargetModelPath(sourceModelPath, targetModelPath) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Couldn't identify target file.");
        return -1;
    }

    if (Compile(pScene, targetModelPath))
    {
        OnResourceBuilt(GetFile(), targetModelPath);
        OnAssetCompiled(GetFile());
        return 0;
    }
    else
    {
        OnAssetCompilationFailed(GetFile(), "Failed to compile file.");
        return -1;
    }
}

bool ModelComp::GetSourceModelPath(const std::filesystem::path& assetPath, std::filesystem::path& sourceModelPath) const
{
    using namespace nlohmann;
    std::ifstream file(assetPath);
    if (file.good())
    {
        json j;
        file >> j;
        file.close();

        json::iterator it = j.find("source");
        if (it != j.end() && it->is_string())
        {
            sourceModelPath = assetPath;
            sourceModelPath = sourceModelPath.remove_filename() / it->get<std::string>();
            return true;
        }
        else
        {
            Core::Log::Error() << "Couldn't find required 'source' field in asset " << assetPath;
            return false;
        }
    }
    return false;
}

bool ModelComp::GetTargetModelPath(const std::filesystem::path& sourceModelPath, std::filesystem::path& targetModelPath) const
{
    std::filesystem::path targetPath = GetDataDir() / std::filesystem::relative(GetFile(), GetAssetsDir()).remove_filename();
    std::filesystem::create_directories(targetPath);
    const std::string targetFileName = sourceModelPath.stem().string() + ".gmdl";
    targetModelPath = targetPath / targetFileName;
    return true;
}

bool ModelComp::Compile(const aiScene* pScene, std::filesystem::path& targetModelPath)
{
    std::ofstream file(targetModelPath, std::ios::out | std::ios::trunc);
    if (file.good())
    {
        WriteHeader(file, pScene);
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
    file << static_cast<uint8_t>(pScene->mNumMeshes);
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
