// Copyright 2014 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <glm/glm.hpp>
#include <list>

namespace Hyperscape
{

class TrailPointData
{
public:
    TrailPointData(const glm::vec3& position, float width, float lifetime)
        : m_Position(position)
        , m_Width(width)
        , m_Lifetime(lifetime)
    {
    }
    const glm::vec3& GetPosition() const;
    float GetWidth() const;
    void SetWidth(float value);
    float GetLifetime() const;
    void SetLifetime(float value);

    float DistanceTo(const TrailPointData& point) const;

private:
    glm::vec3 m_Position;
    float m_Width;
    float m_Lifetime;
};

inline const glm::vec3& TrailPointData::GetPosition() const
{
    return m_Position;
}

inline float TrailPointData::GetWidth() const
{
    return m_Width;
}

inline void TrailPointData::SetWidth(float width)
{
    m_Width = width;
}

inline float TrailPointData::GetLifetime() const
{
    return m_Lifetime;
}

inline void TrailPointData::SetLifetime(float value)
{
    m_Lifetime = value;
}

inline float TrailPointData::DistanceTo(const TrailPointData& point) const
{
    return glm::distance(m_Position, point.GetPosition());
}

typedef std::list<TrailPointData> TrailPointDataList;

} // namespace Hyperscape
