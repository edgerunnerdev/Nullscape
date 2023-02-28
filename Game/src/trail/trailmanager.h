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

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec4.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <coredefines.h>
#include <scene/sceneobject.h>

#include <list>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Trail);

using TrailList = std::list<TrailSharedPtr>;

class TrailManager : public Genesis::SceneObject
{
public:
    TrailManager() {}
    ~TrailManager() {}

    void Update(float delta) override;
    void Render( const Genesis::SceneCameraSharedPtr& pCamera ) override {}
    const TrailList& GetTrails() const;
    TrailWeakPtr Add(float initialWidth, float decay, const glm::vec4& color);
    void Remove(TrailWeakPtr pTrail);

private:
    TrailList m_Trails;
};

inline const TrailList& TrailManager::GetTrails() const
{
    return m_Trails;
}

} // namespace Hyperscape
