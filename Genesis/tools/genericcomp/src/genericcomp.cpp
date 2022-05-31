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

#include <log.hpp>
#include <json.hpp>
#include <fstream>
#include "genericcomp.hpp"

namespace Genesis
{
namespace ResComp
{

GenericComp::GenericComp() {}
GenericComp::~GenericComp() {}

int GenericComp::Run()
{
    std::filesystem::path targetPath = GetDataDir() / std::filesystem::relative(GetFile(), GetAssetsDir()).remove_filename();
    std::filesystem::create_directories(targetPath);

    using namespace nlohmann;
    json j;

    std::filesystem::path sourceFile;
    std::ifstream file(GetFile());
    if (file.good())
    {
        json j;
        file >> j;

        json::iterator it = j.find("source");
        if (it != j.end() && it->is_string())
        {
            std::filesystem::path filePath(GetFile());
            sourceFile = filePath.remove_filename() / it->get<std::string>();
        }
        file.close();
    }

    if (std::filesystem::exists(sourceFile) == false)
    {
        Core::Log::Error() << "Source file " << sourceFile << "doesn't exist.";
        return -1;
    }

    std::filesystem::path targetFile = targetPath / sourceFile.filename();
    bool fileCopied = std::filesystem::copy_file(sourceFile, targetFile, std::filesystem::copy_options::overwrite_existing);
    if (fileCopied)
    {
        OnResourceBuilt(GetFile(), targetFile);
        OnAssetCompiled(GetFile());
        return 0;
    }
    else
    {
        OnAssetCompilationFailed(GetFile(), "File copy failed.");
        return -1;
    }
}

} // namespace ResComp
} // namespace Genesis
