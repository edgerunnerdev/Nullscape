// Copyright 2023 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <vector>

#include <coredefines.h>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <imgui/imgui.h>
#include <externalheadersend.hpp>
// clang-format on

#include "ui2/fonts.hpp"

namespace Nullscape::UI2
{

GENESIS_DECLARE_SMART_PTR( Window );

namespace Private
{
    extern std::array<ImFont*, static_cast<size_t>( UI2::FontId::Count )> sRegisteredFonts;
    extern std::vector<WindowWeakPtr> sWindows;

    void RegisterDefaultFont();
    void RegisterFont( FontId id, const std::filesystem::path& path, float size );
    void SetupStyle();

} // namespace Private
} // namespace Nullscape::UI2
