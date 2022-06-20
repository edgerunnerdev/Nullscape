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

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/ext/std_map.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <externalheadersend.hpp>
// clang-format on

#include <string>

namespace Genesis::Serialization
{

static const size_t sMaxEntries = 1000000;

struct ModelHeader
{
    std::string format;
    uint8_t version;
    uint8_t materials;
    uint8_t meshes;
};

template <typename S> void serialize(S& s, ModelHeader& header)
{
    s.text1b(header.format, 4);
    s.value1b(header.version);
    s.value1b(header.materials);
    s.value1b(header.meshes);
}

struct ModelMaterial
{
    std::string name;
    std::string shader;
    std::unordered_map<std::string, std::string> bindings;
};

template <typename S> void serialize(S& s, ModelMaterial& material)
{
    s.text1b(material.name, 256);
    s.text1b(material.shader, 256);
    s.ext(material.bindings, bitsery::ext::StdMap{256},
          [](S& s, std::string& key, std::string& value)
          {
              s.text1b(key, 256);
              s.text1b(value, 256);
          });
}

struct MeshHeader
{
    uint8_t materialIndex;
    uint32_t vertices;
    uint32_t triangles;
    uint32_t uvChannels;
};

template <typename S> void serialize(S& s, MeshHeader& meshHeader)
{
    s.value1b(meshHeader.materialIndex);
    s.value4b(meshHeader.vertices);
    s.value4b(meshHeader.triangles);
    s.value4b(meshHeader.uvChannels);
}

struct Vertex
{
    float x;
    float y;
    float z;
};

template <typename S> void serialize(S& s, Vertex& vertex)
{
    s.value4b(vertex.x);
    s.value4b(vertex.y);
    s.value4b(vertex.z);
}

struct Triangle
{
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
};

template <typename S> void serialize(S& s, Triangle& triangle)
{
    s.value4b(triangle.v1);
    s.value4b(triangle.v2);
    s.value4b(triangle.v3);
}

struct UV
{
    float u;
    float v;
};

template <typename S> void serialize(S& s, UV& uv)
{
    s.value4b(uv.u);
    s.value4b(uv.v);
}

struct UVChannel
{
    std::vector<UV> uvs;
};

template <typename S> void serialize(S& s, UVChannel& uvChannel)
{
    s.container(uvChannel.uvs, sMaxEntries);
}

struct Normal
{
    float x;
    float y;
    float z;
};

template <typename S> void serialize(S& s, Normal& normal)
{
    s.value4b(normal.x);
    s.value4b(normal.y);
    s.value4b(normal.z);
}

struct Mesh
{
    MeshHeader header;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<UVChannel> uvChannels;
    std::vector<Normal> normals;
};

template <typename S> void serialize(S& s, Mesh& mesh)
{
    s.object(mesh.header);
    s.container(mesh.vertices, sMaxEntries);
    s.container(mesh.triangles, sMaxEntries);
    s.container(mesh.uvChannels, 256);
    s.container(mesh.normals, sMaxEntries);
}

struct Model
{
    ModelHeader header;
    std::vector<ModelMaterial> materials;
    std::vector<Mesh> meshes;
};

template <typename S> void serialize(S& s, Model& model)
{
    s.object(model.header);
    s.container(model.materials, 256);
    s.container(model.meshes, 256);
}

} // namespace Genesis::Serialization
