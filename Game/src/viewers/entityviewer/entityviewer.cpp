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

#include <genesis.h>
#include <render/rendertarget.h>
#include <render/viewport.hpp>
#include <rendersystem.h>
#include <resources/resourcemodel.h>
#include <scene/light.h>
#include <scene/scene.h>

#include "viewers/entityviewer/entityviewer.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <sstream>

#include "entity/entity.hpp"
#include "entity/component.hpp"
#include "entity/componentfactory.hpp"

namespace Hyperscape
{

static const int sViewportWidth = 800;
static const int sViewportHeight = 800;

EntityViewer::EntityViewer()
    : m_IsOpen(false)
    , m_Pitch(0.0f)
    , m_Yaw(-90.0f)
    , m_Position(0.0f, 0.0f, 200.0f)
{
    using namespace Genesis;
    ImGuiImpl::RegisterMenu("Game", "Entity viewer", &m_IsOpen);

    m_pViewport = std::make_shared<Viewport>("Entity viewer", sViewportWidth, sViewportHeight, true, false);
    FrameWork::GetRenderSystem()->AddViewport(m_pViewport);

    Scene* pScene = m_pViewport->GetScene();
    m_pBackgroundLayer = pScene->AddLayer(1, true);
    m_pMainLayer = pScene->AddLayer(2);

    LightArray& lights = pScene->GetLights();
    lights[0].SetPosition({100.0f, 100.0f, 100.0f});
    lights[1].SetPosition({100.0f, 0.0f, 0.0f});
    lights[2].SetPosition({-100.0f, 100.0f, -100.0f});

    m_pDebugRender = new Render::DebugRender();
    m_pMainLayer->AddSceneObject(m_pDebugRender, true);

    m_pFileViewer = std::make_unique<FileViewer>(300, sViewportHeight, "data/templates", ".json");

    m_pEntity = std::make_unique<Entity>();
}

EntityViewer::~EntityViewer()
{
    Genesis::FrameWork::GetRenderSystem()->RemoveViewport(m_pViewport);
}

void EntityViewer::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        ImGui::Begin("Entity viewer", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::BeginChild("File viewer", ImVec2(300, sViewportHeight), true);
        m_pFileViewer->Render();
        ImGui::EndChild();

        ImGui::SameLine();

        Genesis::RenderTarget* pRenderTarget = m_pViewport->GetRenderTarget();
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pRenderTarget->GetColor())),
                     ImVec2(static_cast<float>(pRenderTarget->GetWidth()), static_cast<float>(pRenderTarget->GetHeight())), ImVec2(0, 1), // UV1
                     ImVec2(1, 0),                                                                                                        // UV2
                     ImVec4(1, 1, 1, 1),                                                                                                  // Tint
                     ImVec4(1, 1, 1, 1)                                                                                                   // Border
        );

        UpdateCamera(ImGui::IsItemHovered());

        ImGui::SameLine();

        ImGui::BeginChild("Components", ImVec2(300, sViewportHeight), true);
        
        if (ImGui::BeginMenu("Add component"))
        {
            for (size_t i = 0; i < static_cast<size_t>(ComponentType::Count); ++i)
            {
                ComponentType type = static_cast<ComponentType>(i);
                auto name = magic_enum::enum_name(type);
                if (ImGui::MenuItem(name.data())) 
                {
                    m_pEntity->AddComponent(std::move(ComponentFactory::Get()->Create(type)));
                }
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        std::vector<Component*> components = m_pEntity->GetComponents();
        for (Component* pComponent : components)
        {
            ImGui::PushID(pComponent);
            auto name = magic_enum::enum_name(pComponent->GetType());
            if (ImGui::TreeNode(name.data()))
            {
                pComponent->UpdateDebugUI();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        ImGui::EndChild();

        ImGui::End();

        if (m_pFileViewer->HasSelected())
        {
            //LoadModel(m_pFileViewer->GetSelected());
        }
    }
}

void EntityViewer::UpdateCamera(bool acceptInput)
{
    Genesis::Camera* pCamera = m_pViewport->GetScene()->GetCamera();

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
        const float speed = io.KeyShift ? 100.0f : 10.0f;

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

} // namespace Hyperscape
