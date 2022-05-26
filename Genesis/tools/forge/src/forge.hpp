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

#include <filesystem>
#include <unordered_set>
#include <vector>

namespace Genesis
{
namespace ResComp
{

class Asset;
using AssetVector = std::vector<Asset>;

class Forge
{
public:
    enum class Mode
    {
        Standalone,
        Service
    };

    Forge(Mode mode, const std::filesystem::path& assetsDir, const std::filesystem::path& dataDir, const std::filesystem::path& intermediatesDir);
    ~Forge();

    bool Run();

private:
    bool InitializeDirectories();
    void AggregateKnownAssets();

    Mode m_Mode;
    std::filesystem::path m_AssetsDir;
    std::filesystem::path m_DataDir;
    std::filesystem::path m_IntermediatesDir;
    AssetVector m_KnownAssets;
};

} // namespace ResComp
} // namespace Genesis
