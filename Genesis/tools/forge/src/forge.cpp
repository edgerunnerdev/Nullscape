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

#include "forge.hpp"

#include "asset.hpp"
#include "cache.hpp"
#include "compiler.hpp"
#include "filewatcher.hpp"

#include <filesystem>
#include <log.hpp>
#include <platform.hpp>
#include <process.hpp>
#include <sstream>
#include <stringhelpers.hpp>
#include <thread>
#include <chrono>

namespace Genesis
{
namespace ResComp
{

Forge::Forge(Mode mode, const std::filesystem::path& assetsDir, const std::filesystem::path& compilersDir, const std::filesystem::path& dataDir, const std::filesystem::path& intermediatesDir)
    : m_Mode(mode)
    , m_AssetsDir(assetsDir)
    , m_CompilersDir(compilersDir)
    , m_DataDir(dataDir)
    , m_IntermediatesDir(intermediatesDir)
    , m_QuitRequested(false)
{
}

Forge::~Forge()
{
    if (m_pRPCServer)
    {
        m_pRPCServer->close_sessions();
        m_pRPCServer->stop();
    }
}

bool Forge::Run()
{
    if (!InitializeDirectories())
    {
        Log::Error() << "Failed to initialize directories.";
        return false;
    }

    AggregateCompilers();
    AggregateKnownAssets();

    InitializeCache();
    InitializeRPCServer();

    for (auto& compiler : m_CompilersMap)
    {
        Log::Info() << "Compiler found: " << compiler.first;
    }

    if (m_Mode == Mode::Standalone)
    {
        return CompileAssets();
    }
    else if (m_Mode == Mode::Service)
    {
        if (InitializeFileWatcher() == false)
        {
            Log::Error() << "Failed to initialize file watcher.";
            return false;
        }

        while (m_QuitRequested == false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        return true;
    }
    else
    {
        return false;
    }
}

CompilerSharedPtr Forge::FindCompiler(const std::string& compilerName) const 
{
    CompilersMap::const_iterator it = m_CompilersMap.find(compilerName);
    return (it == m_CompilersMap.end()) ? nullptr : it->second;
}

bool Forge::InitializeDirectories()
{
    if (std::filesystem::is_directory(m_AssetsDir) == false)
    {
        Log::Error() << "Invalid asset directory: " << m_AssetsDir;
        return false;
    }
    else if (std::filesystem::is_directory(m_CompilersDir) == false)
    {
        Log::Error() << "Invalid compilers directory: " << m_CompilersDir;
        return false;
    }
    else if (std::filesystem::is_directory(m_DataDir) == false)
    {
        Log::Error() << "Invalid data directory: " << m_DataDir;
        return false;
    }
    else if (std::filesystem::is_directory(m_IntermediatesDir) == false && std::filesystem::create_directories(m_IntermediatesDir) == false)
    {
        Log::Error() << "Invalid intermediates directory: " << m_IntermediatesDir;
        return false;
    }
    else
    {
        m_AssetsDir = std::filesystem::canonical(m_AssetsDir);
        m_CompilersDir = std::filesystem::canonical(m_CompilersDir);
        m_DataDir = std::filesystem::canonical(m_DataDir);
        m_IntermediatesDir = std::filesystem::canonical(m_IntermediatesDir);
        return true;
    }
}

void Forge::AggregateCompilers()
{
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_CompilersDir))
    {
        const std::filesystem::path path = dirEntry.path();
        const std::string fileName = path.stem().generic_string();

#ifdef TARGET_PLATFORM_WINDOWS
        const bool isExecutable = (path.extension() == ".exe");
#else
        const bool isExecutable = (std::filesystem::status(path).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
#endif
        const bool isCompiler = Genesis::StringEndsWith(fileName, "Comp");
        if (isExecutable && isCompiler)
        {
            CompilerSharedPtr pCompiler = std::make_shared<Compiler>(path);
            m_CompilersMap[pCompiler->GetName()] = pCompiler;
        }
    }
}

void Forge::AggregateKnownAssets()
{
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_AssetsDir))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".asset")
        {
            m_KnownAssets.emplace_back(this, dirEntry);
        }
    }
}

void Forge::InitializeCache() 
{
    m_pCache = std::make_unique<Cache>(m_IntermediatesDir);
}

bool Forge::InitializeFileWatcher() 
{
    m_pFileWatcher = std::make_unique<FileWatcher>();

    m_pFileWatcher->changeEvent = [this](int64_t id, const std::set<std::pair<std::wstring, uint32_t>>& notifications)
    {
        // We don't specifically care about what has changed, just that something has happened and that we need to evaluate our assets.
        bool compilationNeeded = false;
        for (const auto& notif : notifications)
        {
            if (notif.second == FILE_ACTION_MODIFIED)
            {
                Log::Info() << "Change detected on '" << notif.first << "'.";
                compilationNeeded = true;
            }
        }

        if (compilationNeeded)
        {
            CompileAssets();
        }
    };

    m_pFileWatcher->errorEvent = [](int64_t id)
    {
        Log::Error() << "An error has occurred with file watcher, no further events will be sent for ID= " << id;
    };

    if (m_pFileWatcher->AddDirectory(1, m_AssetsDir))
    {
        Log::Info() << "Listening for changes on assets directory.";
        return true;
    }
    else
    {
        Log::Error() << "Failed to listen for changes on assets directory.";
        return false;
    }
}

void Forge::InitializeRPCServer()
{
    m_pRPCServer = std::make_unique<rpc::server>(FORGE_PROCESS_PORT);
    m_pRPCServer->async_run(2);
    Log::Info() << "Initialized RPC server on port " << FORGE_PROCESS_PORT << ".";

    m_pRPCServer->bind("cache",
                       [this](const std::string& asset, const std::string& sourceFile, const std::string& destinationFile)
                       {
                           OnResourceBuilt(asset, sourceFile, destinationFile);
                       });

    m_pRPCServer->bind("failed",
                       [this](const std::string& asset, const std::string& reason)
                       {
                           OnAssetCompilationFailed(asset, reason);
                       });

    m_pRPCServer->bind("log",
                       [this](const std::string& text, int level)
                       {
                           Log::Level logLevel = static_cast<Log::Level>(level);
                           if (logLevel == Log::Level::Info)
                           {
                               Log::Info() << text;
                           }
                           else if (logLevel == Log::Level::Warning)
                           {
                               Log::Warning() << text;
                           }
                           else if (logLevel == Log::Level::Error)
                           {
                               Log::Error() << text;
                           }
                       });

    m_pRPCServer->bind("quit",
                       [this]()
                       {
                           m_QuitRequested = true;
                       });
}

bool Forge::CompileAssets()
{
    Log::Info() << "Compiling assets...";
    int errors = 0;
    int compiled = 0;
    int cached = 0;

    for (Asset& asset : m_KnownAssets)
    {
        CompileResult result = CompileAsset(&asset);
        if (result == CompileResult::Error)
        {
            errors++;
        }
        else if (result == CompileResult::Success)
        {
            compiled++;
        }
        else if (result == CompileResult::Cached)
        {
            cached++;
        }
    }

    Log::Info() << "Forge asset compilation completed, " << compiled << " compiled, " << cached << " cached, " << errors << " errors.";

    return errors == 0;
}

Forge::CompileResult Forge::CompileAsset(Asset* pAsset) 
{
    if (pAsset->IsValid() == false)
    {
        Log::Error() << "Failed to compile " << pAsset->GetPath() << ": Invalid asset.";
        return CompileResult::Error;
    }
    else if (m_pCache->NeedsRebuild(pAsset))
    {
        Log::Info() << "Compiling " << pAsset->GetPath() << "...";

        std::stringstream arguments;
        arguments << "-a " << m_AssetsDir << " -d " << m_DataDir << " -f " << pAsset->GetPath() << " -m forge";

        // Log::Info() << it->second << " " << arguments.str();

        Process process(pAsset->GetCompiler()->GetPath(), arguments.str());
        process.Run();
        process.Wait();
        if (process.GetExitCode() != 0)
        {
            Log::Error() << "Failed to compile " << pAsset->GetPath() << ", process exited with error code " << static_cast<int>(process.GetExitCode());
            return CompileResult::Error;
        }
        else
        {
            return CompileResult::Success;
        }
    }
    else
    {
        return CompileResult::Cached;
    }
}

void Forge::OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& sourceFile, const std::filesystem::path& destinationFile)
{
    Log::Info() << asset << ": " << sourceFile << " -> " << destinationFile;
    for (Asset& knownAsset : m_KnownAssets)
    {
        if (knownAsset.GetPath() == asset)
        {
            m_pCache->Add(&knownAsset);
            break;
        }
    }
}

void Forge::OnAssetCompilationFailed(const std::filesystem::path& asset, const std::string& reason)
{
    Log::Error() << "Failed to compile " << asset << ": " << reason;
}

} // namespace ResComp
} // namespace Genesis
