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
    , m_Pitch(0.0f)
    , m_Yaw(-90.0f)
    , m_Position(0.0f, 0.0f, 200.0f)
{
    using namespace Genesis;
    ImGuiImpl::RegisterMenu("Tools", "Model viewer", &m_IsOpen);

    static int sViewportWidth = 800;
    static int sViewportHeight = 800;

    m_pViewport = std::make_shared<Genesis::Viewport>(sViewportWidth, sViewportHeight);
    FrameWork::GetRenderSystem()->AddViewport(m_pViewport);

    Scene* pScene = m_pViewport->GetScene();
    m_pBackgroundLayer = pScene->AddLayer(1, true);
    m_pMainLayer = pScene->AddLayer(2);

    ModelViewerBackground* pBackground = new ModelViewerBackground(sViewportWidth, sViewportHeight);
    m_pBackgroundLayer->AddSceneObject(pBackground, true);

    m_pDebugRender = new Render::DebugRender();
    m_pMainLayer->AddSceneObject(m_pDebugRender, true);

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
        ImGui::Begin("Model viewer", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        Genesis::RenderTarget* pRenderTarget = m_pViewport->GetRenderTarget();
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pRenderTarget->GetColor())),
                     ImVec2(static_cast<float>(pRenderTarget->GetWidth()), static_cast<float>(pRenderTarget->GetHeight())), ImVec2(0, 1), // UV1
                     ImVec2(1, 0),                                                                                                        // UV2
                     ImVec4(1, 1, 1, 1),                                                                                                  // Tint
                     ImVec4(1, 1, 1, 1)                                                                                                   // Border
        );

        UpdateCamera(ImGui::IsItemHovered());

        ImGui::Text("Position: %.2f %.2f %.2f", m_Position.x, m_Position.y, m_Position.z);

        ImGui::End();

        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(200.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 200.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void ModelViewer::UpdateCamera(bool acceptInput) 
{
    using namespace Genesis;
    Camera* pCamera = m_pViewport->GetScene()->GetCamera();
    
    ImGuiIO& io = ImGui::GetIO();
    if (acceptInput && io.MouseDownDuration[0] > 0.05f)
    {
        static const float sMouseDeltaScale = 0.2f;
        m_Yaw += io.MouseDelta.x * sMouseDeltaScale;
        m_Pitch -= io.MouseDelta.y * sMouseDeltaScale;
    }
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    direction.y = sin(glm::radians(m_Pitch));
    direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    if (acceptInput)
    {
        const float speed = io.KeyShift ? 100.0f : 10.0f;;

        if (io.KeysDown[SDLK_w])
        {
            m_Position += direction * io.DeltaTime * speed;
        }
        else if (io.KeysDown[SDLK_s])
        {
            m_Position -= direction * io.DeltaTime * speed;
        }

        const glm::vec3 right = glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f));
        if (io.KeysDown[SDLK_a])
        {
            m_Position -= right * io.DeltaTime * speed;
        }
        else if (io.KeysDown[SDLK_d])
        {
            m_Position += right * io.DeltaTime * speed;
        }
    }

    pCamera->SetPosition(m_Position);
    pCamera->SetTargetPosition(m_Position + direction);
}

} // namespace Hexterminate::UI::Debug
