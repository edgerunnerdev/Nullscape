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

#include "resources/forgelistener.hpp"

#include "genesis.h"
#include "imgui/imgui.h"
#include "log.hpp"
#include "platform.hpp"
#include "resourcemanager.h"

#include <filesystem>
#include <sstream>
#include <string>

namespace Genesis
{

ForgeListener::ForgeListener()
{
    // We act as a server, so the Forge process can notify us when an asset has been compiled.
    Log::Info() << "Starting Forge Listener on port " << FORGE_LISTENER_PORT;
    m_pRPCServer = std::make_unique<rpc::server>("127.0.0.1", FORGE_LISTENER_PORT);
    m_pRPCServer->async_run();

    Log::Info() << "Initialized RPC server on port " << FORGE_PROCESS_PORT << ".";

    m_pRPCServer->bind("resource_built",
                       [this](const std::string& resourceFile)
                       {
                           OnResourceBuilt(resourceFile);
                       });

    SpawnForgeProcess();
}

ForgeListener::~ForgeListener()
{
    if (m_pRPCClient)
    {
        Log::Info() << "Telling Forge process to quit.";
        m_pRPCClient->send("quit");
    }

    if (m_pRPCServer)
    {
        Log::Info() << "Stopping Forge Listener.";
        m_pRPCServer->close_sessions();
        m_pRPCServer->stop();
    }
}

void ForgeListener::Update() 
{
    if (!m_RebuiltResources.empty())
    {
        std::lock_guard<std::mutex> lock(m_RebuiltResourcesMutex);
        for (ResourceGeneric* pResource : m_RebuiltResources)
        {
            Log::Info() << "Built resource '" << pResource->GetFilename().GetFullPath() << "'.";
            pResource->OnForgeBuild();
        }
        m_RebuiltResources.clear();
    }
}

void ForgeListener::SpawnForgeProcess()
{
    using namespace std::filesystem;

#ifdef TARGET_PLATFORM_WINDOWS
    path forgePath("../../Genesis/bin/Forge.exe");
#else
    path forgePath("../../Genesis/bin/Forge");
#endif

    if (exists(forgePath))
    {
        // We act as a client so we can notify the Forge process that it should quit when the game terminates.
        m_pRPCClient = std::make_unique<rpc::client>("127.0.0.1", FORGE_PROCESS_PORT);

        path assetsPath = current_path() / "assets";
        path dataPath = current_path() / "data";
        path intermediatesPath = current_path() / "intermediates";
        path compilersPath = canonical(current_path() / ".." / ".." / "Genesis" / "bin" / "compilers");

        std::stringstream arguments;
        arguments << "-m service -a " << assetsPath.generic_string() << " -d " << dataPath.generic_string() << " -c " << compilersPath.generic_string() << " -i " << intermediatesPath.generic_string();

        m_pProcess = std::make_unique<Process>(forgePath, arguments.str());
        m_pProcess->Run();
    }
}

void ForgeListener::OnResourceBuilt(const std::string& resourceFile) 
{
    size_t t = resourceFile.rfind("/data/");
    if (t == std::string::npos)
    {
        Log::Warning() << "Resource '" << resourceFile << "' has been built, but it's not in the expected 'data' directory.";
    }
    else
    {
        const std::string relativeResourceFile(resourceFile.substr(t + 1)); // converts the absolute path to e.g. /data/shaders/antiproton.glsl
        ResourceGeneric* pResource = FrameWork::GetResourceManager()->GetResource(relativeResourceFile);
        if (pResource)
        {
            std::lock_guard<std::mutex> lock(m_RebuiltResourcesMutex);
            m_RebuiltResources.push_back(pResource);
        }
        else
        {
            Log::Warning() << "Built resource '" << relativeResourceFile << "', but it's not loaded.";
        }
    }
}

} // namespace Genesis
