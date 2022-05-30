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

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <rpc/client.h>
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
    void OnResourceBuilt(const std::filesystem::path& asset, const std::filesystem::path& resource);
    void OnAssetCompiled(const std::filesystem::path& asset);
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
