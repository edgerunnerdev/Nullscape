// Copyright 2014 Pedro Nunes
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

#include "genesis.h"

#include "configuration.h"
#include "eventhandler.h"
#include "gui/gui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl.h"
#include "inputmanager.h"
#include "memory.h"
#include "render/debugrender.h"
#include "render/viewport.hpp"
#include "rendersystem.h"
#include "resourcemanager.h"
#include "scene/scene.h"
#include "sound/soundmanager.h"
#include "taskmanager.h"
#include "timer.h"
#include "window.h"

#include <log.hpp>
#include <SDL_image.h>

namespace Genesis
{

//---------------------------------------------------------------
// Globals
//---------------------------------------------------------------

TaskManager* gTaskManager = nullptr;
InputManager* gInputManager = nullptr;
EventHandler* gEventHandler = nullptr;
Window* gWindow = nullptr;
RenderSystem* gRenderSystem = nullptr;
ResourceManager* gResourceManager = nullptr;
Gui::GuiManager* gGuiManager = nullptr;
Sound::SoundManager* gSoundManager = nullptr;
VideoPlayer* gVideoPlayer = nullptr;
Render::DebugRender* gDebugRender = nullptr;

CommandLineParameters* FrameWork::m_pCommandLineParameters = nullptr;

//-------------------------------------------------------------------
// FrameWork
//-------------------------------------------------------------------

bool FrameWork::Initialize()
{
    Log::AddLogTarget(std::make_shared<FileLogger>("log.txt"));
    Log::AddLogTarget(std::make_shared<MessageBoxLogger>());

    // Initialize SDL
    // Needs to be done before InputManager() is created,
    // otherwise key repetition won't work.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
    {
        Log::Error() << SDL_GetError();
    }
    else
    {
        Log::Info() << "SDL initialized.";
    }

    Configuration::Load();

    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int imgResult = IMG_Init(flags);
    if ((imgResult & flags) != flags)
    {
        if ((flags & IMG_INIT_JPG) == 0)
        {
            Log::Error() << "SDL2_image is unable to load JPGs.";
        }

        if ((flags & IMG_INIT_PNG) == 0)
        {
            Log::Error() << "SDL2_image is unable to load PNGs.";
        }

        Log::Error() << "IMG_Init failed.";
        exit(-1);
    }
    else
    {
        Log::Info() << "SDL2_image initialized with JPG and PNG support.";
    }

    gInputManager = new InputManager();
    gEventHandler = new EventHandler();
    gResourceManager = new ResourceManager();

    // Initialize the task manager, as well as all the related tasks
    gTaskManager = new TaskManager();
    gTaskManager->AddTask("InputManager", gInputManager, (TaskFunc)&InputManager::Update, TaskPriority::System);
    gTaskManager->AddTask("EventHandler", gEventHandler, (TaskFunc)&EventHandler::Update, TaskPriority::System);
    gTaskManager->AddTask("ResourceManager", gResourceManager, (TaskFunc)&ResourceManager::Update, TaskPriority::System);

    gRenderSystem = new RenderSystem();
    gTaskManager->AddTask("Render", gRenderSystem, (TaskFunc)&RenderSystem::Update, TaskPriority::Rendering);

    gGuiManager = new Gui::GuiManager();
    gTaskManager->AddTask("GUIManager", gGuiManager, (TaskFunc)&Gui::GuiManager::Update, TaskPriority::GameLogic);

    gSoundManager = new Sound::SoundManager();
    gTaskManager->AddTask("SoundManager", gSoundManager, (TaskFunc)&Sound::SoundManager::Update, TaskPriority::System);

    return true;
}

void FrameWork::Shutdown()
{
    delete gDebugRender;
    gDebugRender = nullptr;

    delete gGuiManager;
    gGuiManager = nullptr;

    delete gVideoPlayer;
    gVideoPlayer = nullptr;

    delete gInputManager;
    gInputManager = nullptr;

    delete gEventHandler;
    gEventHandler = nullptr;

    delete gTaskManager;
    gTaskManager = nullptr;

    delete gRenderSystem;
    gRenderSystem = nullptr;

    delete gResourceManager;
    gResourceManager = nullptr;

    delete gSoundManager;
    gSoundManager = nullptr;

    delete gWindow;
    gWindow = nullptr;
}

bool FrameWork::CreateWindowGL(const std::string& name, uint32_t width, uint32_t height, uint32_t multiSampleSamples /* = 0 */)
{
    // Set OpenGL version to 3.3.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // OpenGL core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);                          // OpenGL 3+
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);                          // OpenGL 3.3

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    if (multiSampleSamples > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multiSampleSamples);
    }

    gWindow = new Window(name, width, height, Configuration::IsFullscreen());

    GetRenderSystem()->Initialize(width, height);

    GetGuiManager()->Initialize();
    ImGuiImpl::Initialise();

    gDebugRender = new Render::DebugRender();

    return true;
}

CommandLineParameters* FrameWork::CreateCommandLineParameters(const char* parameterStr)
{
    m_pCommandLineParameters = new CommandLineParameters(parameterStr);
    return m_pCommandLineParameters;
}

CommandLineParameters* FrameWork::CreateCommandLineParameters(const char** parameters, uint32_t numParameters)
{
    m_pCommandLineParameters = new CommandLineParameters(parameters, numParameters);
    return m_pCommandLineParameters;
}

CommandLineParameters* FrameWork::GetCommandLineParameters()
{
    return m_pCommandLineParameters;
}

TaskManager* FrameWork::GetTaskManager()
{
    return gTaskManager;
}

InputManager* FrameWork::GetInputManager()
{
    return gInputManager;
}

Window* FrameWork::GetWindow()
{
    return gWindow;
}

RenderSystem* FrameWork::GetRenderSystem()
{
    return gRenderSystem;
}

ResourceManager* FrameWork::GetResourceManager()
{
    return gResourceManager;
}

SceneSharedPtr& FrameWork::GetScene()
{
    return gRenderSystem->GetPrimaryViewport()->GetScene();
}

Gui::GuiManager* FrameWork::GetGuiManager()
{
    return gGuiManager;
}

Sound::SoundManager* FrameWork::GetSoundManager()
{
    return gSoundManager;
}

VideoPlayer* FrameWork::GetVideoPlayer()
{
    return gVideoPlayer;
}

Render::DebugRender* FrameWork::GetDebugRender()
{
    return gDebugRender;
}

//---------------------------------------------------------------
// CommandLineParameters
//---------------------------------------------------------------

CommandLineParameters::CommandLineParameters(const char* parameterStr)
{
    // Do we even have any parameters?
    if (parameterStr != nullptr)
    {
        std::string tmpStr(parameterStr);

        size_t previousPos = 0;
        size_t currentPos = tmpStr.find_first_of(" ", 0);
        // If there is no whitespace, then there's only one parameter
        if (currentPos == std::string::npos)
        {
            mParameters.push_back(parameterStr);
        }
        // Otherwise, process every parameter
        else
        {
            do
            {
                mParameters.push_back(tmpStr.substr(previousPos, currentPos - previousPos));
                previousPos = currentPos + 1;
                currentPos = tmpStr.find_first_of(" ", previousPos);
            } while (currentPos != std::string::npos);

            mParameters.push_back(tmpStr.substr(previousPos, tmpStr.size() - previousPos));
        }
    }
}

CommandLineParameters::CommandLineParameters(const char** parameters, size_t numParameters)
{
    for (size_t i = 0; i < numParameters; i++)
    {
        mParameters.push_back(parameters[i]);
    }
}

bool CommandLineParameters::HasParameter(const std::string& name) const
{
    for (auto& parameter : mParameters)
    {
        if (parameter == name)
        {
            return true;
        }
    }

    return false;
}
} // namespace Genesis