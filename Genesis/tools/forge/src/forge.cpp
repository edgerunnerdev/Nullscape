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
#include "cache.hpp"
#include "compiler.hpp"

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

    InitializeCache();
    InitializeRPCServer();

    for (auto& compiler : m_CompilersMap)
    {
        Log::Info() << "Compiler found: " << compiler.first;
    }

    return CompileAssets();
}

CompilerSharedPtr Forge::FindCompiler(const std::string& compilerName) const 
{
    CompilersMap::const_iterator it = m_CompilersMap.find(compilerName);
    return (it == m_CompilersMap.end()) ? nullptr : it->second;
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

void Forge::InitializeRPCServer()
{
    using namespace Core;
    static const int port = 47563;
    m_pRPCServer = std::make_unique<rpc::server>(port);
    m_pRPCServer->async_run(2);
    Log::Info() << "Initialized RPC server on port " << port << ".";

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
}

bool Forge::CompileAssets()
{
    Core::Log::Info() << "Compiling assets...";
    int errors = 0;
    int compiled = 0;
    int cached = 0;

    for (Asset& asset : m_KnownAssets)
    {
        if (asset.IsValid() == false)
        {
            Core::Log::Error() << "Failed to compile " << asset.GetPath() << ": Invalid asset.";
            errors++;
        }
        else if (m_pCache->NeedsRebuild(&asset))
        {
            Core::Log::Info() << "Compiling " << asset.GetPath() << "...";

            std::stringstream arguments;
            arguments << "-a " << m_AssetsDir << " -d " << m_DataDir << " -f " << asset.GetPath() << " -m forge";

            //Core::Log::Info() << it->second << " " << arguments.str();

            Core::Process process(asset.GetCompiler()->GetPath(), arguments.str());
            process.Run();
            process.Wait();
            if (process.GetExitCode() != 0)
            {
                Core::Log::Error() << "Failed to compile " << asset.GetPath() << ", process exited with error code " << static_cast<int>(process.GetExitCode());
                errors++;
            }
            else
            {
                compiled++;
            }
        }
        else
        {
            cached++;
        }
    }

    Core::Log::Info() << "Forge asset compilation completed, " << compiled << " compiled, " << cached << " cached, " << errors << " errors.";

    return errors == 0;
}

void Forge::OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& sourceFile, const std::filesystem::path& destinationFile)
{
    Core::Log::Info() << asset << ": " << sourceFile << " -> " << destinationFile;
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
    Core::Log::Error() << "Failed to compile " << asset << ": " << reason;
}

} // namespace ResComp
} // namespace Genesis
