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

namespace Genesis
{
namespace ResComp
{

Forge::Forge(Mode mode, const std::filesystem::path& assetsDir, const std::filesystem::path& dataDir, const std::filesystem::path& intermediatesDir)
    : m_Mode(mode)
    , m_AssetsDir(assetsDir)
    , m_DataDir(dataDir)
    , m_IntermediatesDir(intermediatesDir)
{
}

Forge::~Forge() {}

bool Forge::Run()
{
    using namespace Genesis::Core;

    if (!InitializeDirectories())
    {
        Log::Error("Failed to initialize directories.");
        return false;
    }

    AggregateKnownAssets();

    return false;
}

bool Forge::InitializeDirectories()
{
    using namespace Genesis::Core;
    if (std::filesystem::is_directory(m_AssetsDir) == false)
    {
        Log::Error("Invalid asset directory: %s.", m_AssetsDir.c_str());
        return false;
    }
    else if (std::filesystem::is_directory(m_DataDir) == false)
    {
        Log::Error("Invalid data directory: %s.", m_DataDir.c_str());
        return false;
    }
    else if (std::filesystem::is_directory(m_IntermediatesDir) == false && std::filesystem::create_directories(m_IntermediatesDir) == false)
    {
        Log::Error("Invalid intermediates directory: %s.", m_IntermediatesDir.c_str());
        return false;
    }
    else
    {
        return true;
    }
}

void Forge::AggregateKnownAssets()
{
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_AssetsDir))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == "asset")
        {
            Asset asset(dirEntry);
            m_KnownAssets.push_back(std::move(asset));
        }
    }
}

} // namespace ResComp
} // namespace Genesis
