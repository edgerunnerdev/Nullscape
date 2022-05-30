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

#include "rescomp.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <cmdparser.hpp>
#include <SDL.h>
#include <externalheadersend.hpp>
// clang-format on

#include <log.hpp>
#include "forgelogger.hpp"

namespace Genesis
{
namespace ResComp
{

ResComp::ResComp()
    : m_UsingForge(false)
{
}

ResComp::~ResComp() {}

bool ResComp::Initialize(int argc, char** argv)
{
    cli::Parser parser(argc, argv);
    parser.set_optional<std::string>("m", "mode", "standalone", "Mode for the compiler to run in. Can be 'standalone' or 'forge'.");
    parser.set_required<std::string>("a", "assets-dir", "Assets directory, containing all assets to be converted.");
    parser.set_required<std::string>("d", "data-dir", "Data directory, to which compiled resources will be written to.");
    parser.set_required<std::string>("f", "file", "File to compile.");
    parser.run_and_exit_if_error();

    std::string modeArg = parser.get<std::string>("m");
    if (modeArg == "standalone")
    {
        m_UsingForge = false;
    }
    else if (modeArg == "forge")
    {
        m_UsingForge = true;
    }
    else
    {
        Core::Log::Error() << "Mode " << modeArg << " not recognized, must be 'standalone' or 'service'.";
        return false;
    }

    if (IsUsingForge())
    {
        static const int port = 47563;
        m_pRPCClient = std::make_unique<rpc::client>("127.0.0.1", port);
        Core::Log::AddLogTarget(std::make_shared<ForgeLogger>(m_pRPCClient.get()));
    }
    else
    {
        Core::Log::AddLogTarget(std::make_shared<Core::TTYLogger>());
    }

    m_AssetsDir = std::filesystem::canonical(std::filesystem::path(parser.get<std::string>("a")));
    m_DataDir = std::filesystem::canonical(std::filesystem::path(parser.get<std::string>("d")));
    m_File = std::filesystem::canonical(std::filesystem::path(parser.get<std::string>("f")));

    return true;
}

int ResComp::Run()
{
    return 0;
}

const std::filesystem::path& ResComp::GetAssetsDir() const
{
    return m_AssetsDir;
}

const std::filesystem::path& ResComp::GetDataDir() const
{
    return m_DataDir;
}

const std::filesystem::path& ResComp::GetFile() const
{
    return m_File;
}

bool ResComp::IsUsingForge() const
{
    return m_UsingForge;
}

void ResComp::OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& resource)
{
    //Core::Log::Info() << asset << ": build resource " << resource;
}

void ResComp::OnAssetCompiled(const std::filesystem::path& asset)
{
    if (IsUsingForge())
    {
        m_pRPCClient->send("success", asset.lexically_normal().generic_string());
    }
    //Core::Log::Info() << "Compiled asset " << asset;
}

void ResComp::OnAssetCompilationFailed(const std::filesystem::path& asset, const std::string& reason)
{
    //Core::Log::Error() << "Failed to compile asset " << asset << ": " << reason;
}

} // namespace ResComp
} // namespace Genesis
