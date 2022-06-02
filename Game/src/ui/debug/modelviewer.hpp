// Copyright 2022 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <render/debugrender.h>
#include <render/viewport.hpp>
#include <scene/layer.h>
#include <rendersystem.fwd.h>

#include "ui/types.fwd.h"

namespace Hexterminate::UI::Debug
{

class ModelViewer
{
public:
    ModelViewer();
    virtual ~ModelViewer();

    void UpdateDebugUI();

private:
    void UpdateCamera(bool acceptInput);

    bool m_IsOpen;
    Genesis::ViewportSharedPtr m_pViewport;
    Genesis::LayerSharedPtr m_pBackgroundLayer;
    Genesis::LayerSharedPtr m_pMainLayer;
    Genesis::Render::DebugRender* m_pDebugRender;
    float m_Pitch;
    float m_Yaw;
    glm::vec3 m_Position;
};

} // namespace Hexterminate::UI::Debug
