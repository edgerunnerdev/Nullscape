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

#include "entity/entitytemplateeditor.hpp"

#include <genesis.h>
#include <render/rendertarget.h>
#include <render/viewport.hpp>
#include <rendersystem.h>
#include <resources/resourcemodel.h>
#include <scene/light.h>
#include <scene/scene.h>
#include <scene/scenecamera.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <sstream>

#include "entity/entity.hpp"
#include "entity/component.hpp"
#include "entity/componentfactory.hpp"
#include "entity/entityfactory.hpp"
#include "sprite/spritemanager.h"
#include "trail/trailmanager.h"
#include "trail/trailmanagerrep.h"

namespace Hyperscape
{

static const int sViewportWidth = 1024;
static const int sViewportHeight = 1024;

EntityTemplateEditor::EntityTemplateEditor()
    : m_IsOpen(false)
    , m_Pitch(0.0f)
    , m_Yaw(-90.0f)
    , m_Position(0.0f, 0.0f, 200.0f)
{
    using namespace Genesis;
    ImGuiImpl::RegisterDevMenu("Game", "Entity template editor", &m_IsOpen);

    m_pScene = std::make_shared<Scene>();
    m_pCamera = std::make_shared<SceneCamera>();
    m_pScene->AddCamera( m_pCamera );
    m_pViewport = std::make_shared<Viewport>( "Entity template edito", sViewportWidth, sViewportHeight, m_pScene, m_pCamera, true, false );
    FrameWork::GetRenderSystem()->AddViewport(m_pViewport);

    SceneSharedPtr pScene = m_pViewport->GetScene();
    m_pBackgroundLayer = pScene->AddLayer(1, true);
    m_pMainLayer = pScene->AddLayer(2);

    LightArray& lights = pScene->GetLights();
    lights[0].SetPosition({100.0f, 100.0f, 100.0f});
    lights[1].SetPosition({100.0f, 0.0f, 0.0f});
    lights[2].SetPosition({-100.0f, 100.0f, -100.0f});

    m_pDebugRender = new Render::DebugRender();
    m_pMainLayer->AddSceneObject(m_pDebugRender, true);
    m_pMainLayer->AddSceneObject(new SpriteManager(), true);

    TrailManager* pTrailManager = new TrailManager();
    m_pMainLayer->AddSceneObject(pTrailManager, true);
    m_pMainLayer->AddSceneObject(new TrailManagerRep(pTrailManager), true);
}

EntityTemplateEditor::~EntityTemplateEditor()
{
    Genesis::FrameWork::GetRenderSystem()->RemoveViewport(m_pViewport);
}

void EntityTemplateEditor::UpdateDebugUI()
{
    if (Genesis::ImGuiImpl::IsEnabled() && m_IsOpen)
    {
        ImGui::Begin("Entity template editor", &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

        DrawMenu();

        ImGui::BeginChild("Template list", ImVec2(300, sViewportHeight), true);
        DrawTemplateList();
        ImGui::EndChild();

        ImGui::SameLine();

        Genesis::RenderTargetSharedPtr pRenderTarget = m_pViewport->GetRenderTarget();
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pRenderTarget->GetColor())),
                     ImVec2(static_cast<float>(pRenderTarget->GetWidth()), static_cast<float>(pRenderTarget->GetHeight())), ImVec2(0, 1), // UV1
                     ImVec2(1, 0),                                                                                                        // UV2
                     ImVec4(1, 1, 1, 1),                                                                                                  // Tint
                     ImVec4(1, 1, 1, 1)                                                                                                   // Border
        );

        UpdateCamera(ImGui::IsItemHovered());

        ImGui::SameLine();

        ImGui::BeginChild("Components", ImVec2(600, sViewportHeight), true);
        
        ImGui::TextUnformatted("Components");
        ImGui::Separator();

        if (m_pEntity == nullptr)
        {
            ImGui::TextDisabled("No template loaded.");
        }
        else
        {        
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
        }

        ImGui::EndChild();

        ImGui::End();
    }
}

void EntityTemplateEditor::DrawMenu() 
{
    static std::string sNewTemplateName;
    bool openNewTemplatePopup = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Template"))
        {
            if (ImGui::MenuItem("New"))
            {
                openNewTemplatePopup = true;
            }

            bool canSave = (m_pEntity != nullptr);
            if (ImGui::MenuItem("Save", nullptr, nullptr, canSave))
            {
                EntityFactory::Get()->SaveTemplate(m_LoadedTemplate, m_pEntity);
            }

            ImGui::EndMenu();
        }
    
        if (ImGui::BeginMenu("Add component", m_pEntity != nullptr))
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

        ImGui::EndMenuBar();
    }

    if (openNewTemplatePopup)
    {
        sNewTemplateName = "";
        ImGui::OpenPopup("New template");
    }
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("New template", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Template name", &sNewTemplateName);

        if (ImGui::Button("Create", ImVec2(200, 0)))
        {
            EntityFactory::Get()->AddBlankTemplate(sNewTemplateName);
            LoadTemplate(sNewTemplateName);
            sNewTemplateName = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void EntityTemplateEditor::DrawTemplateList() 
{
    ImGui::TextUnformatted("Templates");
    ImGui::Separator();
    std::set<std::string> templates = EntityFactory::Get()->GetTemplateNames();
    for (const std::string& templateName : templates)
    {
        bool selected = (templateName == m_LoadedTemplate);
        if (ImGui::Selectable(templateName.c_str(), &selected) && m_LoadedTemplate != templateName)
        {
            LoadTemplate(templateName);
        }
    }
}

void EntityTemplateEditor::UpdateCamera(bool acceptInput)
{
    Genesis::SceneCameraSharedPtr pCamera = m_pViewport->GetScene()->GetCamera();

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

void EntityTemplateEditor::LoadTemplate(const std::string& templateName) 
{
    if (m_pEntity != nullptr)
    {
        m_pMainLayer->RemoveSceneObject(m_pEntity.get());
        m_pEntity = nullptr;
    }

    m_pEntity = EntityFactory::Get()->Create(templateName);
    m_pEntity->SetEditorEntity(true);
    m_pMainLayer->AddSceneObject(m_pEntity.get(), false);
    m_LoadedTemplate = templateName;
}

} // namespace Hyperscape
