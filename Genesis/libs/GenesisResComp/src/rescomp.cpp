/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "rescomp.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <cmdparser.hpp>
#include <SDL.h>
#include <externalheadersend.hpp>
// clang-format on

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
        // Log::Error() << "Mode " << modeArg << " not recognized, must be 'standalone' or 'service'.";
        return false;
    }

    if (IsUsingForge())
    {
        static const int port = 47563;
        m_pRPCClient = std::make_unique<rpc::client>("127.0.0.1", port);
    }

    m_AssetsDir = parser.get<std::string>("a");
    m_DataDir = parser.get<std::string>("d");
    m_File = parser.get<std::string>("f");

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
