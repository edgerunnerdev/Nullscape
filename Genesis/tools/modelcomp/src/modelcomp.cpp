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

#include "modelcomp.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/string.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/convex_hull_3.h>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "material.hpp"
#include "modelserialization.hpp"

#include <fstream>
#include <log.hpp>

namespace Genesis::ResComp
{

ModelComp::ModelComp()
    : m_GeneratePhysicsMesh(false)
{
}

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
    else if (ValidateImport(pScene) == false)
    {
        OnAssetCompilationFailed(GetFile(), "Import validation failed.");
        return -1;
    }
    else if (ValidateMaterials(pScene) == false)
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
            Log::Error() << "Couldn't find required 'source' field in asset " << assetPath;
            return false;
        }
        else
        {
            m_SourceModelPath = assetPath;
            m_SourceModelPath = m_SourceModelPath.remove_filename() / it->get<std::string>();
        }

        it = j.find("generate_physics_mesh");
        if (it != j.end() && it->is_boolean())
        {
            m_GeneratePhysicsMesh = it->get<bool>();
        }

        it = j.find("materials");
        if (it == j.end() || it->is_array() == false)
        {
            Log::Error() << "Couldn't find required 'materials' array in asset " << assetPath;
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
                    Log::Error() << "Couldn't find material name in asset " << assetPath;
                    return false;
                }
                else
                {
                    pMaterial->SetName(materialIt->get<std::string>());
                }

                materialIt = jMaterial.find("shader");
                if (materialIt == jMaterial.end() || materialIt->is_string() == false)
                {
                    Log::Error() << "Couldn't find required 'shader' string for material " << pMaterial->GetName() << " in asset " << assetPath;
                    return false;
                }
                else
                {
                    pMaterial->SetShader(materialIt->get<std::string>());
                }

                materialIt = jMaterial.find("bindings");
                if (materialIt == jMaterial.end() || materialIt->is_array() == false)
                {
                    Log::Error() << "Couldn't find required 'bindings' array for material " << pMaterial->GetName() << " in asset " << assetPath;
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
                            Log::Error() << "Couldn't find required 'name' string in bindings for material " << pMaterial->GetName() << " in asset " << assetPath;
                            return false;
                        }
                        std::string bindingName = bindingIt->get<std::string>();

                        bindingIt = jBinding.find("filename");
                        if (bindingIt == jBinding.end() || bindingIt->is_string() == false)
                        {
                            Log::Error() << "Couldn't find required 'filename' string in bindings for material " << pMaterial->GetName() << " in asset " << assetPath;
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

bool ModelComp::ValidateImport(const aiScene* pScene)
{
    if (pScene->HasMeshes() == false)
    {
        Log::Error() << "Model has no meshes.";
        return false;
    }

    for (int i = 0; i < pScene->mNumMeshes; ++i)
    {
        aiMesh* pMesh = pScene->mMeshes[i];
        if (pMesh->HasPositions() == false)
        {
            Log::Error() << "Mesh " << pMesh->mName.C_Str() << " has no vertex positions.";
            return false;
        }
        else if (pMesh->HasNormals() == false)
        {
            Log::Error() << "Mesh " << pMesh->mName.C_Str() << " has no normals.";
            return false;
        }
        else if (pMesh->HasTextureCoords(0) == false)
        {
            Log::Error() << "Mesh " << pMesh->mName.C_Str() << " has no texture coordinate set 0.";
            return false;
        }
        else if (pMesh->HasTangentsAndBitangents() == false)
        {
            Log::Error() << "Mesh " << pMesh->mName.C_Str() << " has no tangents / bitangents.";
            return false;
        }
        else if (pMesh->HasFaces() == false)
        {
            Log::Error() << "Mesh " << pMesh->mName.C_Str() << " has no faces.";
            return false;
        }

        for (int j = 0; j < pMesh->mNumFaces; ++j)
        {
            const aiFace& face = pMesh->mFaces[j];
            if (face.mNumIndices != 3)
            {
                Log::Error() << "Mesh " << pMesh->mName.C_Str() << " is not triangulated.";
                return false;
            }
        }
    }

    return true;
}

bool ModelComp::ValidateMaterials(const aiScene* pScene)
{
    bool mismatch = false;
    if (m_Materials.size() != pScene->mNumMaterials)
    {
        Log::Error() << "Different number of materials between model file and asset.";
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
        Log::Error() << "Material mismatch between asset and model file.";

        Log::Error() << "Asset:";
        for (auto& pMaterial : m_Materials)
        {
            Log::Error() << "- " << pMaterial.first;
        }

        Log::Error() << "Model file:";
        for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
        {
            Log::Error() << "- " << pScene->mMaterials[i]->GetName().C_Str();
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

bool ModelComp::Compile(const aiScene* pImportedScene, std::filesystem::path& targetModelPath)
{
    std::ofstream file(targetModelPath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (file.good())
    {
        bool success = true;
        Serialization::Model model;
        success &= WriteHeader(model, pImportedScene);
        success &= WriteMaterials(model);
        success &= WriteMeshes(model, pImportedScene);

        if (m_GeneratePhysicsMesh)
        {
            success &= WritePhysicsMesh(model, pImportedScene);
        }

        if (success)
        {
            bitsery::Serializer<bitsery::OutputBufferedStreamAdapter> ser{file};
            ser.object(model);
            ser.adapter().flush();

            file.close();
            return true;
        }
        else
        {
            file.close();
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool ModelComp::WriteHeader(Serialization::Model& model, const aiScene* pImportedScene)
{
    model.header.format = "GMDL";
    model.header.version = 2;
    model.header.materials = static_cast<uint8_t>(pImportedScene->mNumMaterials);
    model.header.meshes = static_cast<uint8_t>(pImportedScene->mNumMeshes);
    model.header.hasPhysicsMesh = m_GeneratePhysicsMesh ? 1 : 0;
    return true;
}

bool ModelComp::WriteMaterials(Serialization::Model& model)
{
    for (auto& materialPair : m_Materials)
    {
        Material* pMaterial = materialPair.second.get();
        Serialization::ModelMaterial material;
        material.name = pMaterial->GetName();
        material.shader = pMaterial->GetShader();
        material.bindings = pMaterial->GetBindings();
        model.materials.push_back(std::move(material));
    }
    return true;
}

bool ModelComp::WriteMeshes(Serialization::Model& model, const aiScene* pImportedScene)
{
    for (unsigned int i = 0; i < pImportedScene->mNumMeshes; ++i)
    {
        const aiMesh* pImportedMesh = pImportedScene->mMeshes[i];
        Serialization::Mesh mesh;
        WriteMeshHeader(mesh, pImportedMesh);
        WriteMesh(mesh, pImportedMesh);
        model.meshes.push_back(std::move(mesh));
    }
    return true;
}

void ModelComp::WriteMeshHeader(Serialization::Mesh& mesh, const aiMesh* pImportedMesh)
{
    mesh.header.materialIndex = pImportedMesh->mMaterialIndex;
    mesh.header.vertices = pImportedMesh->mNumVertices;
    mesh.header.triangles = pImportedMesh->mNumFaces;
    mesh.header.uvChannels = pImportedMesh->GetNumUVChannels();
}

void ModelComp::WriteMesh(Serialization::Mesh& mesh, const aiMesh* pImportedMesh)
{
    aiMatrix4x4 rotX;
    aiMatrix4x4::RotationX(-AI_MATH_HALF_PI, rotX);

    mesh.vertices.reserve(mesh.header.vertices);
    for (uint32_t i = 0; i < mesh.header.vertices; ++i)
    {
        aiVector3D vertex = rotX * pImportedMesh->mVertices[i];
        mesh.vertices.push_back({vertex.x, vertex.y, vertex.z});
    }

    mesh.triangles.reserve(mesh.header.triangles);
    for (uint32_t i = 0; i < mesh.header.triangles; ++i)
    {
        const aiFace& face = pImportedMesh->mFaces[i];
        mesh.triangles.push_back({face.mIndices[0], face.mIndices[1], face.mIndices[2]});
    }

    mesh.uvChannels.reserve(mesh.header.uvChannels);
    for (uint32_t i = 0; i < mesh.header.uvChannels; ++i)
    {
        Serialization::UVChannel uvChannel;
        for (uint32_t j = 0; j < mesh.header.vertices; ++j)
        {
            const aiVector3D& uv = pImportedMesh->mTextureCoords[i][j];
            uvChannel.uvs.push_back({uv.x, 1.0f - uv.y});
        }
        mesh.uvChannels.push_back(std::move(uvChannel));
    }

    mesh.normals.reserve(mesh.header.vertices);
    mesh.tangents.reserve(mesh.header.vertices); 
    mesh.bitangents.reserve(mesh.header.vertices);
    for (uint32_t i = 0; i < mesh.header.vertices; ++i)
    {
        aiVector3D normal = rotX * pImportedMesh->mNormals[i];
        mesh.normals.push_back({normal.x, normal.y, normal.z});

        aiVector3D tangent = rotX * pImportedMesh->mTangents[i];
        mesh.tangents.push_back({tangent.x, tangent.y, tangent.z});

        aiVector3D bitangent = rotX * pImportedMesh->mBitangents[i];
        mesh.bitangents.push_back({bitangent.x, bitangent.y, bitangent.z});
    }
}

bool ModelComp::WritePhysicsMesh(Serialization::Model& model, const aiScene* pImportedScene)
{
    using K = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Polyhedron_3 = CGAL::Polyhedron_3<K>;
    using Point_3 = K::Point_3;
    using Surface_mesh = CGAL::Surface_mesh<Point_3>;

    uint32_t numTriangles = 0;
    std::vector<Point_3> points;
    aiMatrix4x4 rotX;
    aiMatrix4x4::RotationX(-AI_MATH_HALF_PI, rotX);

    for (unsigned int i = 0; i < pImportedScene->mNumMeshes; ++i)
    {
        const aiMesh* pImportedMesh = pImportedScene->mMeshes[i];
        const size_t numVertices = pImportedMesh->mNumVertices;
        numTriangles += pImportedMesh->mNumFaces;
        points.reserve(points.size() + numVertices);
        for (uint32_t j = 0; j < numVertices; ++j)
        {
            aiVector3D vertex = rotX * pImportedMesh->mVertices[j];
            points.push_back({vertex.x, vertex.y, vertex.z});
        }
    }

    // define polyhedron to hold convex hull
    Polyhedron_3 poly;
    Surface_mesh sm;
    // compute convex hull of non-collinear points
    CGAL::convex_hull_3(points.begin(), points.end(), poly);
    CGAL::convex_hull_3(points.begin(), points.end(), sm);

    Serialization::Mesh mesh;

    mesh.vertices.reserve(poly.size_of_vertices());
    for (const auto& vertex : poly.points())
    {
        mesh.vertices.push_back({static_cast<float>(vertex.x()), static_cast<float>(vertex.y()), static_cast<float>(vertex.z())});
    }

    for (auto& faceIndex : sm.faces())
    {
        auto verticesAroundFace = sm.vertices_around_face(sm.halfedge(faceIndex));
        if (verticesAroundFace.size() != 3)
        {
            Log::Error() << "Generated physics mesh with non-triangular face.";
            return false;
        }

        uint32_t faceIndices[3] = {0, 0, 0};
        int i = 0;
        for (const auto& vertexIndex : verticesAroundFace)
        {
            faceIndices[i++] = vertexIndex.idx();
        }
        mesh.triangles.push_back({faceIndices[0], faceIndices[1], faceIndices[2]});
    }

    mesh.header.materialIndex = 0;
    mesh.header.uvChannels = 0;
    mesh.header.vertices = static_cast<uint32_t>(mesh.vertices.size());
    mesh.header.triangles = static_cast<uint32_t>(mesh.triangles.size());

    Log::Info() << "Generated physics convex hull with " << mesh.header.vertices << " vertices and " << mesh.header.triangles << " triangles from base mesh with " << points.size() << " vertices and "
                << numTriangles << " triangles.";

    model.physicsMesh = std::move(mesh);
    return true;
}

} // namespace Genesis::ResComp
