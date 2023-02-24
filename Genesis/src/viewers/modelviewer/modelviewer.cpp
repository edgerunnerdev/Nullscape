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

#include "viewers/modelviewer/modelviewer.hpp"

#include <sstream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl.h"
#include "viewers/fileviewer/fileviewer.hpp"
#include "viewers/modelviewer/modelviewerbackground.hpp"
#include "viewers/modelviewer/modelviewerobject.hpp"
#include "resources/resourcemodel.h"
#include "genesis.h"
#include "render/rendertarget.h"
#include "render/viewport.hpp"
#include "rendersystem.h"
#include "scene/light.h"
#include "scene/scene.h"

namespace Genesis
{

static const int sViewportWidth = 800;
static const int sViewportHeight = 800;

ModelViewer::ModelViewer()
    : m_IsOpen(false)
    , m_Pitch(0.0f)
    , m_Yaw(-90.0f)
    , m_Position(0.0f, 0.0f, 200.0f)
    , m_pModel(nullptr)
    , m_DrawLights(false)
    , m_DrawAxis(true)
    , m_DrawNormals(false)
    , m_DrawTangents(false)
    , m_DrawBitangents(false)
    , m_DrawPhysicsMesh(false)
{
    ImGuiImpl::RegisterDevMenu("Tools", "Model viewer", &m_IsOpen);

    m_pViewport = std::make_shared<Viewport>("Model viewer", sViewportWidth, sViewportHeight, true, false);
    FrameWork::GetRenderSystem()->AddViewport(m_pViewport);

    Scene* pScene = m_pViewport->GetScene();
    m_pBackgroundLayer = pScene->AddLayer(1, true);
    m_pMainLayer = pScene->AddLayer(2);

    LightArray& lights = pScene->GetLights();
    lights[0].SetPosition({100.0f, 100.0f, 100.0f});
    lights[1].SetPosition({100.0f, 0.0f, 0.0f});
    lights[2].SetPosition({-100.0f, 100.0f, -100.0f});

    ModelViewerBackground* pBackground = new ModelViewerBackground(sViewportWidth, sViewportHeight);
    m_pBackgroundLayer->AddSceneObject(pBackground, true);

    m_pDebugRender = new Render::DebugRender();
    m_pMainLayer->AddSceneObject(m_pDebugRender, true);

    m_pFileViewer = std::make_unique<FileViewer>("data/models", ".gmdl");
}

ModelViewer::~ModelViewer()
{
    FrameWork::GetRenderSystem()->RemoveViewport(m_pViewport);
}

void ModelViewer::UpdateDebugUI()
{
    if (ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        ImGui::Begin("Model viewer", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::BeginChild("File viewer", ImVec2(300, sViewportHeight), true);
        m_pFileViewer->Render();
        ImGui::EndChild();

        ImGui::SameLine();

        RenderTargetSharedPtr pRenderTarget = m_pViewport->GetRenderTarget();
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pRenderTarget->GetColor())),
                     ImVec2(static_cast<float>(pRenderTarget->GetWidth()), static_cast<float>(pRenderTarget->GetHeight())), ImVec2(0, 1), // UV1
                     ImVec2(1, 0),                                                                                                        // UV2
                     ImVec4(1, 1, 1, 1),                                                                                                  // Tint
                     ImVec4(1, 1, 1, 1)                                                                                                   // Border
        );
        
        DrawAxis();
        DrawDebugModel();
        DrawLights();

        UpdateCamera(ImGui::IsItemHovered());

        ImGui::SameLine();

        ImGui::BeginChild("Properties", ImVec2(300, sViewportHeight), true);
        ShowStats();
        ShowOverlays();
        ShowLights();
        ImGui::EndChild();

        ImGui::End();

        if (m_pFileViewer->HasSelected())
        {
            LoadModel(m_pFileViewer->GetSelected());
        }
    }
}

void ModelViewer::DrawAxis() 
{
    if (m_DrawAxis)
    {
        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(200.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 200.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_pDebugRender->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void ModelViewer::DrawDebugModel() 
{
    if (m_pModel)
    {
        ResourceModel::DebugRenderFlags flags = ResourceModel::DebugRenderFlags::None;
        if (m_DrawNormals)
        {
            flags |= ResourceModel::DebugRenderFlags::Normals;
        }
        if (m_DrawTangents)
        {
            flags |= ResourceModel::DebugRenderFlags::Tangents;
        }
        if (m_DrawBitangents)
        {
            flags |= ResourceModel::DebugRenderFlags::Bitangents;
        }
        if (m_DrawPhysicsMesh)
        {
            flags |= ResourceModel::DebugRenderFlags::PhysicsMesh;
        }
        m_pModel->DebugRender(m_pDebugRender, flags);
    }
}

void ModelViewer::DrawLights() 
{
    if (m_DrawLights)
    {
        for (const Light& light : m_pViewport->GetScene()->GetLights())
        {
            m_pDebugRender->DrawLine(light.GetPosition(), glm::vec3(0.0f), light.GetColor());
        }
    }
}

void ModelViewer::UpdateCamera(bool acceptInput) 
{
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

void ModelViewer::LoadModel(const std::filesystem::path& path) 
{
    if (m_pModel != nullptr)
    {
        m_pMainLayer->RemoveSceneObject(m_pModel);
    }

    m_pModel = new ModelViewerObject(path);
    m_pMainLayer->AddSceneObject(m_pModel);
}

void ModelViewer::ShowStats() 
{
    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
    {
        size_t triangleCount = m_pModel ? m_pModel->GetTriangleCount() : 0;
        ImGui::Text("Triangles: %lu", triangleCount);

        size_t vertexCount = m_pModel ? m_pModel->GetVertexCount() : 0;
        ImGui::Text("Vertices: %lu", vertexCount);
    }
}

void ModelViewer::ShowOverlays() 
{
    if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Draw axis", &m_DrawAxis);
        ImGui::Checkbox("Draw normals", &m_DrawNormals);
        ImGui::Checkbox("Draw tangents", &m_DrawTangents);
        ImGui::Checkbox("Draw bitangents", &m_DrawBitangents);
        ImGui::Checkbox("Draw physics mesh", &m_DrawPhysicsMesh);
    }
}

void ModelViewer::ShowLights() 
{
    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Draw lights", &m_DrawLights);

        LightArray& lights = m_pViewport->GetScene()->GetLights();
        size_t numLights = lights.size();
        for (size_t i = 0; i < numLights; ++i)
        {
            std::stringstream ss;
            ss << "Directional light " << i + 1;
            if (ImGui::TreeNode(ss.str().c_str()))
            {
                lights[i].DebugDraw();
                ImGui::TreePop();
            }
        }
    }
}

} // namespace Genesis
