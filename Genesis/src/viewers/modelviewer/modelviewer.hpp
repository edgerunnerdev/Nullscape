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

#include <filesystem>

#include "render/debugrender.h"
#include "render/viewport.hpp"
#include "scene/layer.h"
#include "rendersystem.fwd.h"

namespace Genesis
{

class ModelViewerObject;

class ModelViewer
{
public:
    ModelViewer();
    virtual ~ModelViewer();

    void UpdateDebugUI();

private:
    void LoadModel(const std::filesystem::path& path);
    void UpdateCamera(bool acceptInput);
    void ShowStats();

    bool m_IsOpen;
    ViewportSharedPtr m_pViewport;
    LayerSharedPtr m_pBackgroundLayer;
    LayerSharedPtr m_pMainLayer;
    Render::DebugRender* m_pDebugRender;
    float m_Pitch;
    float m_Yaw;
    glm::vec3 m_Position;
    ModelViewerObject* m_pModel;
};

} // namespace Genesis
