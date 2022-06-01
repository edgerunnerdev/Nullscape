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

#include "ui/debug/modelviewer.hpp"
#include "ui/debug/modelviewerbackground.hpp"
#include "ui/debug/modelviewerobject.hpp"

#include "hexterminate.h"

#include <genesis.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <render/rendertarget.h>
#include <render/viewport.hpp>
#include <rendersystem.h>
#include <scene/scene.h>

namespace Hexterminate::UI::Debug
{

ModelViewer::ModelViewer()
    : m_IsOpen(false)
{
    using namespace Genesis;
    ImGuiImpl::RegisterMenu("Tools", "Model viewer", &m_IsOpen);

    m_pViewport = std::make_shared<Genesis::Viewport>(512, 512);
    FrameWork::GetRenderSystem()->AddViewport(m_pViewport);

    Scene* pScene = m_pViewport->GetScene();
    m_pBackgroundLayer = pScene->AddLayer(1, true);
    m_pMainLayer = pScene->AddLayer(2);

    Camera* pCamera = pScene->GetCamera();
    pCamera->SetPosition(0.0f, 0.0f, 200.0f);
    pCamera->SetTargetPosition(0.0f, 0.0f, 0.0f);

    ModelViewerBackground* pBackground = new ModelViewerBackground(512, 512);
    m_pBackgroundLayer->AddSceneObject(pBackground, true);

    ModelViewerObject* pModelObject = new ModelViewerObject();
    m_pMainLayer->AddSceneObject(pModelObject);
}

ModelViewer::~ModelViewer()
{
    Genesis::FrameWork::GetRenderSystem()->RemoveViewport(m_pViewport);
}

void ModelViewer::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        ImGui::Begin("Model viewer", &m_IsOpen);

        Genesis::RenderTarget* pRenderTarget = m_pViewport->GetRenderTarget();
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pRenderTarget->GetColor())),
                     ImVec2(static_cast<float>(pRenderTarget->GetWidth()), static_cast<float>(pRenderTarget->GetHeight())), ImVec2(0, 0), // UV1
                     ImVec2(1, 1),                                                                                                        // UV2
                     ImVec4(1, 1, 1, 1),                                                                                                  // Tint
                     ImVec4(1, 1, 1, 1)                                                                                                   // Border
        );

        if (ImGui::IsItemHovered())
        {
            UpdateCamera();
        }

        ImGui::End();
    }
}

void ModelViewer::UpdateCamera() 
{
    using namespace Genesis;
    Camera* pCamera = m_pViewport->GetScene()->GetCamera();
    
    //if (ImGui::GetIO().MouseDownDuration[0] > 0.25f)
    //{
        m_RotX += ImGui::GetIO().MouseDelta.x;
    //}

    glm::mat4 m = glm::rotate(glm::mat4(1.0), m_RotX, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 200.0f));
    pCamera->SetPosition(m[3].x, m[3].y, m[3].z);
}

} // namespace Hexterminate::UI::Debug
