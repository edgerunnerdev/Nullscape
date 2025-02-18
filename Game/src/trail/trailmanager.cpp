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

#include "trail/trailmanager.h"

#include "game.hpp"
#include "trail/trail.h"

#include <memory.h>

namespace Hyperscape
{

void TrailManager::Update(float delta)
{
    if (g_pGame->IsPaused() == false)
    {
        m_Trails.remove_if(
            [](TrailSharedPtr pTrail)
            {
                return pTrail->IsOrphan() && pTrail->GetActivePoints() == 0;
            });

        for (auto& pTrail : m_Trails)
        {
            pTrail->Update(delta);
        }
    }
}

TrailWeakPtr TrailManager::Add(float initialWidth, float lifetime, const glm::vec4& color)
{
    TrailSharedPtr pTrail = std::make_shared<Trail>(initialWidth, lifetime, color);
    m_Trails.push_back(pTrail);
    return pTrail;
}

void TrailManager::Remove(TrailWeakPtr pTrail)
{
    m_Trails.remove(pTrail.lock());
}

} // namespace Hyperscape