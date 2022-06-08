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

#pragma once

#include "forge.hpp"

#include "asset.hpp"

#include <filesystem>
#include <log.hpp>
#include <platform.hpp>
#include <process.hpp>
#include <sstream>
#include <stringhelpers.hpp>

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
    using namespace Genesis::Core;

    if (!InitializeDirectories())
    {
        Log::Error() << "Failed to initialize directories.";
        return false;
    }

    AggregateCompilers();
    AggregateKnownAssets();

    InitializeRPCServer();

    for (auto& compiler : m_CompilersMap)
    {
        Log::Info() << "Compiler found: " << compiler.first;
    }

    CompileAssets();

    return false;
}

bool Forge::InitializeDirectories()
{
    using namespace Genesis::Core;
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
        const bool isCompiler = Genesis::Core::StringEndsWith(fileName, "Comp");
        if (isExecutable && isCompiler)
        {
            m_CompilersMap[fileName] = path;
        }
    }
}

void Forge::AggregateKnownAssets()
{
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_AssetsDir))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".asset")
        {
            m_KnownAssets.emplace_back(dirEntry);
        }
    }
}

void Forge::InitializeRPCServer()
{
    using namespace Core;
    static const int port = 47563;
    m_pRPCServer = std::make_unique<rpc::server>(port);
    m_pRPCServer->async_run(2);
    Log::Info() << "Initialized RPC server on port " << port << ".";

    m_pRPCServer->bind("cache",
                       [this](const std::string& asset, const std::string& resource)
                       {
                           OnResourceBuilt(asset, resource);
                       });

    m_pRPCServer->bind("success",
                       [this](const std::string& asset)
                       {
                           OnAssetCompiled(asset);
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
}

void Forge::CompileAssets()
{
    Core::Log::Info() << "Compiling assets...";

    for (Asset& asset : m_KnownAssets)
    {
        Core::Log::Info() << "Compiling " << asset.GetPath() << "...";
        CompilersMap::iterator it = m_CompilersMap.find(asset.GetCompiler());
        if (it == m_CompilersMap.end())
        {
            Core::Log::Error() << "Unable to compile '" << asset.GetPath() << "', can't find compiler '" << asset.GetCompiler() << "'.";
        }
        else
        {
            std::stringstream arguments;
            arguments << "-a " << m_AssetsDir << " -d " << m_DataDir << " -f " << asset.GetPath() << " -m forge";

            Core::Log::Info() << it->second << " " << arguments.str();

            Core::Process process(it->second, arguments.str());
            process.Run();
            process.Wait();
            if (process.GetExitCode() != 0)
            {
                Core::Log::Error() << "Failed to compile " << asset.GetPath() << ", process exited with error code " << static_cast<int>(process.GetExitCode());
            }
        }
    }
}

void Forge::OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& resource)
{
    Core::Log::Info() << asset << ": built resource " << resource;
}

void Forge::OnAssetCompiled(const std::filesystem::path& asset)
{
    Core::Log::Info() << "Compiled " << asset;
}

void Forge::OnAssetCompilationFailed(const std::filesystem::path& asset, const std::string& reason)
{
    Core::Log::Error() << "Failed to compile " << asset << ": " << reason;
}

} // namespace ResComp
} // namespace Genesis
