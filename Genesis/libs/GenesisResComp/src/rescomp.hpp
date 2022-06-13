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
#include <rpc/client.h>
#include <rpc/rpc_error.h>
#include <externalheadersend.hpp>
// clang-format on

#include <filesystem>
#include <memory>

namespace Genesis
{

namespace ResComp
{

using RPCClientUniquePtr = std::unique_ptr<rpc::client>;

class ResComp
{
public:
    ResComp();
    virtual ~ResComp();

    virtual bool Initialize(int argc, char** argv);
    virtual int Run();

    const std::filesystem::path& GetAssetsDir() const;
    const std::filesystem::path& GetDataDir() const;
    const std::filesystem::path& GetFile() const;
    bool IsUsingForge() const;
    void OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& sourceFile, const std::filesystem::path& destinationFile);
    void OnAssetCompilationFailed(const std::filesystem::path& asset, const std::string& reason);

private:
    std::filesystem::path m_AssetsDir;
    std::filesystem::path m_DataDir;
    std::filesystem::path m_File;
    bool m_UsingForge;
    RPCClientUniquePtr m_pRPCClient;
};

} // namespace ResComp
} // namespace Genesis
