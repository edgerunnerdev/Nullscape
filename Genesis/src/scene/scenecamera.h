// Copyright 2018 Pedro Nunes
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

#include <coredefines.h>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec3.hpp>
#include <externalheadersend.hpp>
// clang-format on

namespace Genesis
{

class SceneCamera
{
public:
    SceneCamera();
    virtual ~SceneCamera(){};

    const glm::vec3& GetPosition() const;
    void SetPosition( const glm::vec3& position );
    void SetPosition( float x, float y, float z );

    const glm::vec3& GetTargetPosition() const;
    void SetTargetPosition( const glm::vec3& position );
    void SetTargetPosition( float x, float y, float z );

    void CalculateUpVector();
    const glm::vec3& GetUpVector() const;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Target;
    glm::vec3 m_Up;
};
GENESIS_DECLARE_SMART_PTR( SceneCamera );

inline const glm::vec3& SceneCamera::GetPosition() const
{
    return m_Position;
}

inline void SceneCamera::SetPosition( const glm::vec3& position )
{
    m_Position = position;
}

inline void SceneCamera::SetPosition( float x, float y, float z )
{
    SetPosition( glm::vec3( x, y, z ) );
}

inline const glm::vec3& SceneCamera::GetTargetPosition() const
{
    return m_Target;
}

inline void SceneCamera::SetTargetPosition( const glm::vec3& target )
{
    m_Target = target;
}

inline void SceneCamera::SetTargetPosition( float x, float y, float z )
{
    SetTargetPosition( glm::vec3( x, y, z ) );
}

inline const glm::vec3& SceneCamera::GetUpVector() const
{
    return m_Up;
}

} // namespace Genesis
