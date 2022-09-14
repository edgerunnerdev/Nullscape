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

#include "trail/trail.h"

// clang-format off
#include <externalheadersbegin.hpp>
#include <SDL.h>
#include <externalheadersend.hpp>
// clang-format on

namespace Nullscape
{

Trail::Trail(float initialWidth, float lifetime, const glm::vec4& color)
    : m_InitialWidth(initialWidth)
    , m_InitialLifetime(lifetime)
    , m_Color(color)
    , m_IsOrphan(false)
    , m_ActivePoints(0)
{
    SDL_assert(m_InitialLifetime > 0.0f);
}

void Trail::AddPoint(const glm::vec3& position)
{
    if (m_Data.size() < 2)
    {
        m_Data.push_front(TrailPointData(position, m_InitialWidth, m_InitialLifetime));
    }
    else
    {
        // The first point always remains at the source of the trail
        TrailPointData point(position, m_InitialWidth, m_InitialLifetime);
        m_Data.front() = point;

        // If the first and second points are too far apart, we place a new one
        TrailPointDataList::iterator secondPointIt = ++(m_Data.begin());
        static const float sThreshold = 1.0f;
        if (glm::distance(secondPointIt->GetPosition(), point.GetPosition()) >= sThreshold)
        {
            m_Data.push_front(point);
        }
    }
}

// The trail decays over time.
// The decay is faster if the trail has been orphaned, so it doesn't linger around if
// the source of the trail no longer exists or is no longer powered.
void Trail::Update(float delta)
{
    m_ActivePoints = 0;
    if (m_Data.empty() == false)
    {
        for (TrailPointDataList::iterator it = m_Data.begin(), itEnd = m_Data.end(); it != itEnd;)
        {
            TrailPointData& point = *it;
            const float pointWidth = point.GetWidth();
            if (pointWidth > 0.0f)
            {
                const float fadedWidth = pointWidth * point.GetLifetime() / m_InitialLifetime;
                if (fadedWidth <= 0.0f)
                {
                    it = m_Data.erase(it);
                }
                else
                {
                    point.SetLifetime(point.GetLifetime() - delta);
                    point.SetWidth(fadedWidth);
                    m_ActivePoints++;
                    it++;
                }
            }
        }
    }
}

} // namespace Nullscape