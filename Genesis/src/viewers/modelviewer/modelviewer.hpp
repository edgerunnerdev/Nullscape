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
#include "rendersystem.fwd.h"
#include "scene/layer.h"

namespace Genesis
{

GENESIS_DECLARE_SMART_PTR( Scene );
GENESIS_DECLARE_SMART_PTR( SceneCamera );

class FileViewer;
class ModelViewerObject;

class ModelViewer
{
public:
    ModelViewer();
    virtual ~ModelViewer();

    void UpdateDebugUI();

private:
    void LoadModel( const std::filesystem::path& path );
    void UpdateCamera( bool acceptInput );
    void DrawAxis();
    void DrawLights();
    void DrawDebugModel();
    void ShowStats();
    void ShowOverlays();
    void ShowLights();

    bool m_IsOpen;
    SceneSharedPtr m_pScene;
    SceneCameraSharedPtr m_pCamera;
    ViewportSharedPtr m_pViewport;
    LayerSharedPtr m_pBackgroundLayer;
    LayerSharedPtr m_pMainLayer;
    Render::DebugRender* m_pDebugRender;
    float m_Pitch;
    float m_Yaw;
    glm::vec3 m_Position;
    ModelViewerObject* m_pModel;
    std::unique_ptr<FileViewer> m_pFileViewer;
    bool m_DrawLights;
    bool m_DrawAxis;
    bool m_DrawNormals;
    bool m_DrawTangents;
    bool m_DrawBitangents;
    bool m_DrawPhysicsMesh;
};

} // namespace Genesis
