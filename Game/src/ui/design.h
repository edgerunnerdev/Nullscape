// Copyright 2021 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <filesystem>
#include <memory>
#include <string>

#include "ui/types.fwd.h"

using json = nlohmann::json;

namespace Hyperscape::UI
{

class Design
{
public:
    Design();

    void Save();
    void Load();
    
    json& Get( const std::string& path );

private:
    std::filesystem::path m_Path;
    json m_Data;
};

} // namespace Hyperscape::UI
