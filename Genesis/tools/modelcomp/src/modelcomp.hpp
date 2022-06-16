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

#include <filesystem>
#include <fstream>
#include <memory>
#include <rescomp.hpp>
#include <unordered_map>

struct aiMesh;
struct aiScene;

namespace Genesis
{
namespace ResComp
{

class Material;
using MaterialUniquePtr = std::unique_ptr<Material>;

class ModelComp : public ResComp
{
public:
    ModelComp();
    virtual ~ModelComp() override;

    virtual int Run() override;

private:
    bool ReadAsset(const std::filesystem::path& assetPath);
    bool ValidateMaterials(const aiScene* pScene);
    std::filesystem::path GetTargetModelPath(const std::filesystem::path& sourceModelPath) const;
    bool Compile(const aiScene* pScene, std::filesystem::path& targetModelPath);
    void WriteHeader(std::ofstream& file, const aiScene* pScene);
    void WriteMaterials(std::ofstream& file, const aiScene* pScene);
    void WriteMeshes(std::ofstream& file, const aiScene* pScene);
    void WriteMeshHeader(std::ofstream& file, const aiMesh* pMesh);
    void WriteMesh(std::ofstream& file, const aiMesh* pMesh);

    using MaterialMap = std::unordered_map<std::string, MaterialUniquePtr>;
    MaterialMap m_Materials;
    std::filesystem::path m_SourceModelPath;
};

} // namespace ResComp
} // namespace Genesis
