// Copyright 2014 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <coredefines.h>

#include "trail/trailpointdata.h"

namespace Nullscape
{

GENESIS_DECLARE_SMART_PTR(Trail);

class Trail
{
public:
    Trail(float initialWidth, float lifetime, const glm::vec4& colour);
    ~Trail(){};

    void AddPoint(const glm::mat4x4& transform);
    void AddPoint(const glm::vec3& position);
    void AddPoint(const glm::vec4& position);
    const TrailPointDataList& GetData() const;
    float GetInitialWidth() const;
    void SetInitialWidth(float value);
    const glm::vec4& GetColor() const;
    void Update(float delta);
    void SetOrphan();
    bool IsOrphan() const;
    int GetActivePoints() const;

private:
    TrailPointDataList m_Data;
    float m_InitialWidth;
    float m_InitialLifetime;
    glm::vec4 m_Color;
    bool m_IsOrphan;
    int m_ActivePoints;
};

inline const TrailPointDataList& Trail::GetData() const
{
    return m_Data;
}

inline float Trail::GetInitialWidth() const
{
    return m_InitialWidth;
}

inline void Trail::SetInitialWidth(float value)
{
    m_InitialWidth = value;
}

inline const glm::vec4& Trail::GetColor() const
{
    return m_Color;
}

inline void Trail::SetOrphan()
{
    m_IsOrphan = true;
}

inline bool Trail::IsOrphan() const
{
    return m_IsOrphan;
}

inline int Trail::GetActivePoints() const
{
    return m_ActivePoints;
}


inline void Trail::AddPoint(const glm::mat4x4& transform) 
{
    AddPoint(glm::vec3(transform[3]));
}

inline void Trail::AddPoint(const glm::vec4& position) 
{
    AddPoint(glm::vec3(position));
}

} // namespace Nullscape
