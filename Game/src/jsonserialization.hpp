// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
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

#include "entity/components/hulltype.hpp"

#include <genesis.h>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <vector>

namespace nlohmann
{

template <>
struct adl_serializer<glm::vec2>
{
    static void to_json( json& j, const glm::vec2& v )
    {
        j = json{ v.x, v.y };
    }

    static void from_json( const json& j, glm::vec2& v )
    {
        const std::vector<float> values = j.get<std::vector<float>>();
        v.x = values.at( 0 );
        v.y = values.at( 1 );
    }
};

template <>
struct adl_serializer<glm::vec3>
{
    static void to_json( json& j, const glm::vec3& v )
    {
        j = json{ v.x, v.y, v.z };
    }

    static void from_json( const json& j, glm::vec3& v )
    {
        const std::vector<float> values = j.get<std::vector<float>>();
        v.x = values.at( 0 );
        v.y = values.at( 1 );
        v.z = values.at( 2 );
    }
};

template <>
struct adl_serializer<glm::vec4>
{
    static void to_json( json& j, const glm::vec4& v )
    {
        j = json{ v.x, v.y, v.z, v.w };
    }

    static void from_json( const json& j, glm::vec4& v )
    {
        const std::vector<float> values = j.get<std::vector<float>>();
        v.x = values.at( 0 );
        v.y = values.at( 1 );
        v.z = values.at( 2 );
        v.w = values.at( 3 );
    }
};

template <>
struct adl_serializer<glm::mat4x4>
{
    static void to_json( json& j, const glm::mat4x4& mat )
    {
        j = json{
            { mat[ 0 ].x, mat[ 0 ].y, mat[ 0 ].z, mat[ 0 ].w },
            { mat[ 1 ].x, mat[ 1 ].y, mat[ 1 ].z, mat[ 1 ].w },
            { mat[ 2 ].x, mat[ 2 ].y, mat[ 2 ].z, mat[ 2 ].w },
            { mat[ 3 ].x, mat[ 3 ].y, mat[ 3 ].z, mat[ 3 ].w }
        };
    }

    static void from_json( const json& j, glm::mat4x4& mat )
    {
        const std::vector<std::vector<float>> values = j.get<std::vector<std::vector<float>>>();
        for ( int i = 0; i < 4; ++i )
        {
            mat[ i ].x = values.at( i ).at( 0 );
            mat[ i ].y = values.at( i ).at( 1 );
            mat[ i ].z = values.at( i ).at( 2 );
            mat[ i ].w = values.at( i ).at( 3 );
        }
    }
};

template <>
struct adl_serializer<Hyperscape::HullType>
{
    static void to_json( json& j, const Hyperscape::HullType& v )
    {
        j = json{ magic_enum::enum_name( v ) };
    }

    static void from_json( const json& j, Hyperscape::HullType& v )
    {
        auto type = magic_enum::enum_cast<Hyperscape::HullType>( j.get<std::string>() );
        v = type.value_or( Hyperscape::HullType::Gunship );
    }
};

} // namespace nlohmann
