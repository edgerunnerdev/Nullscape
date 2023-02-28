// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
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

#include "coredefines.h"
#include "render/rendertarget.h"
#include "rendersystem.fwd.h"
#include "scene/scene.h"

#include <memory>
#include <string>

namespace Genesis
{

GENESIS_DECLARE_SMART_PTR( Scene );
GENESIS_DECLARE_SMART_PTR( SceneCamera );

class Viewport
{
public:
    Viewport( const std::string& name, int width, int height, SceneSharedPtr pScene, SceneCameraSharedPtr pCamera, bool hasDepth = true, bool hasStencil = true );
    ~Viewport();

    void Render();

    const std::string& GetName() const;
    RenderTargetSharedPtr GetRenderTarget();
    SceneSharedPtr& GetScene();
    const SceneSharedPtr& GetScene() const;
    SceneCameraSharedPtr& GetCamera();
    const SceneCameraSharedPtr& GetCamera() const;
    int GetWidth() const;
    int GetHeight() const;

private:
    std::string m_Name;
    RenderTargetSharedPtr m_pRenderTarget;
    SceneSharedPtr m_pScene;
    SceneCameraSharedPtr m_pCamera;
    int m_Width;
    int m_Height;
};

inline SceneSharedPtr& Viewport::GetScene()
{
    return m_pScene;
}

inline const SceneSharedPtr& Viewport::GetScene() const
{
    return m_pScene;
}

inline SceneCameraSharedPtr& Viewport::GetCamera()
{
    return m_pCamera;
}

inline const SceneCameraSharedPtr& Viewport::GetCamera() const
{
    return m_pCamera;
}

inline RenderTargetSharedPtr Viewport::GetRenderTarget()
{
    return m_pRenderTarget;
}

inline int Viewport::GetWidth() const
{
    return m_Width;
}

inline int Viewport::GetHeight() const
{
    return m_Height;
}

inline const std::string& Viewport::GetName() const
{
    return m_Name;
}

} // namespace Genesis
