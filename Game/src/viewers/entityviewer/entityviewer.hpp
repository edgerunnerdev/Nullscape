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

#include <render/debugrender.h>
#include <render/viewport.hpp>
#include <rendersystem.fwd.h>
#include <scene/layer.h>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(Entity);

class EntityViewer final
{
public:
    EntityViewer();
    ~EntityViewer();

    void UpdateDebugUI();

private:
    void DrawMenu();
    void DrawTemplateList();
    void UpdateCamera(bool acceptInput);
    void LoadTemplate(const std::string& templateName);

    bool m_IsOpen;
    Genesis::ViewportSharedPtr m_pViewport;
    Genesis::LayerSharedPtr m_pBackgroundLayer;
    Genesis::LayerSharedPtr m_pMainLayer;
    Genesis::Render::DebugRender* m_pDebugRender;
    float m_Pitch;
    float m_Yaw;
    glm::vec3 m_Position;
    std::string m_LoadedTemplate;
    EntitySharedPtr m_pEntity;
};

} // namespace Hyperscape
