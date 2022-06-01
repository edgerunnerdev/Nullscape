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

#include "scene/scene.h"
#include "rendersystem.fwd.h"

#include <memory>
#include <string>

namespace Genesis
{

class Scene;

class Viewport
{
public:
    Viewport(int width, int height);
    ~Viewport();

    void Render();
    RenderTarget* GetRenderTarget();
    Scene* GetScene();
    int GetWidth();
    int GetHeight();

private:
    RenderTargetUniquePtr m_pRenderTarget;
    SceneSharedPtr m_pScene;
    int m_Width;
    int m_Height;
};

} // namespace Genesis
