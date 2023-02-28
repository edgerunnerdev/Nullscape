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

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <SDL.h>
#include <magic_enum.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <coredefines.h>

#include <string>
#include <vector>

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;

namespace Genesis
{

GENESIS_DECLARE_SMART_PTR(Scene);

class TaskManager;
class Timer;
class InputManager;
class RenderSystem;
class ResourceManager;
class Scene;
class CommandLineParameters;
class VideoPlayer;
class Window;

namespace Gui
{
class GuiManager;
}

namespace Render
{
class DebugRender;
}

namespace Sound
{
class SoundManager;
}

class FrameWork
{
public:
    static bool Initialize();
    static void Shutdown();
    static bool CreateWindowGL(const std::string& name, uint32_t width, uint32_t height, uint32_t multiSampleSamples = 0);

    static CommandLineParameters* CreateCommandLineParameters(const char* parameterStr);
    static CommandLineParameters* CreateCommandLineParameters(const char** parameters, uint32_t numParameters);
    static CommandLineParameters* GetCommandLineParameters();

    static TaskManager* GetTaskManager();
    static InputManager* GetInputManager();
    static Window* GetWindow();
    static RenderSystem* GetRenderSystem();
    static ResourceManager* GetResourceManager();
    static SceneSharedPtr& GetScene();
    static Gui::GuiManager* GetGuiManager();
    static Sound::SoundManager* GetSoundManager();
    static VideoPlayer* GetVideoPlayer();
    static Render::DebugRender* GetDebugRender();

private:
    static CommandLineParameters* m_pCommandLineParameters;
};

class CommandLineParameters
{
public:
    CommandLineParameters(const char* parameterStr);
    CommandLineParameters(const char** parameters, size_t numParameters);
    size_t Size() const;
    const std::string& GetParameter(size_t n) const;
    bool HasParameter(const std::string& name) const;

private:
    typedef std::vector<std::string> CommandLineParameter;
    CommandLineParameter mParameters;
};

inline size_t CommandLineParameters::Size() const
{
    return mParameters.size();
}

inline const std::string& CommandLineParameters::GetParameter(size_t n) const
{
    return mParameters.at(n);
}

} // namespace Genesis
