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

// clang-format off
#include <externalheadersbegin.hpp>
#include <rpc/server.h>
#include <externalheadersend.hpp>
// clang-format on

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Genesis
{
namespace ResComp
{

class Asset;
class Cache;
class Compiler;

using AssetVector = std::vector<Asset>;
using CacheUniquePtr = std::unique_ptr<Cache>;
using CompilerSharedPtr = std::shared_ptr<Compiler>;
using CompilersMap = std::unordered_map<std::string, CompilerSharedPtr>;
using RPCServerUniquePtr = std::unique_ptr<rpc::server>;

class Forge
{
public:
    enum class Mode
    {
        Standalone,
        Service
    };

    Forge(Mode mode, const std::filesystem::path& assetsDir, const std::filesystem::path& compilersDir, const std::filesystem::path& dataDir, const std::filesystem::path& intermediatesDir);
    ~Forge();

    bool Run();

    CompilerSharedPtr FindCompiler(const std::string& compilerName) const;

private:
    void OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& sourceFile, const std::filesystem::path& destinationFile);
    void OnAssetCompilationFailed(const std::filesystem::path& asset, const std::string& reason);

    void InitializeCache();
    void InitializeRPCServer();
    bool InitializeDirectories();
    void AggregateKnownAssets();
    void AggregateCompilers();
    bool CompileAssets();

    Mode m_Mode;
    std::filesystem::path m_AssetsDir;
    std::filesystem::path m_CompilersDir;
    std::filesystem::path m_DataDir;
    std::filesystem::path m_IntermediatesDir;
    AssetVector m_KnownAssets;
    CompilersMap m_CompilersMap;
    RPCServerUniquePtr m_pRPCServer;
    CacheUniquePtr m_pCache;
};

} // namespace ResComp
} // namespace Genesis
