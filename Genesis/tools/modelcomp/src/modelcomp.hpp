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

#include "modelserialization.hpp"

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
    bool ValidateImport(const aiScene* pImportedScene);
    bool ValidateMaterials(const aiScene* pImportedScene);
    std::filesystem::path GetTargetModelPath(const std::filesystem::path& sourceModelPath) const;
    bool Compile(const aiScene* pImportedScene, std::filesystem::path& targetModelPath);
    void WriteHeader(Serialization::Model& model, const aiScene* pImportedScene);
    void WriteMaterials(Serialization::Model& model);
    void WriteMeshes(Serialization::Model& model, const aiScene* pImportedScene);
    void WriteMeshHeader(Serialization::Mesh& mesh, const aiMesh* pImportedMesh);
    void WriteMesh(Serialization::Mesh& mesh, const aiMesh* pImportedMesh);

    using MaterialMap = std::unordered_map<std::string, MaterialUniquePtr>;
    MaterialMap m_Materials;
    std::filesystem::path m_SourceModelPath;
};

} // namespace ResComp
} // namespace Genesis
