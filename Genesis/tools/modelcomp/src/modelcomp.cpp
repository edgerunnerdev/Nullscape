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

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <json.hpp>
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
    }

    return -1;
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

} // namespace ResComp
} // namespace Genesis
