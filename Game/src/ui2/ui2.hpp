// Copyright 2023 Pedro Nunes
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

#include <filesystem>
#include <memory>
#include <string>

#include <coredefines.h>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <imgui/imgui.h>
#include <externalheadersend.hpp>
// clang-format on

#include "ui2/fonts.hpp"
#include "ui2/private/ui2.hpp"

namespace Hyperscape::UI2
{

GENESIS_DECLARE_SMART_PTR( Window );

void Initialize();
void PushFont( FontId id );
void PopFont();
ImVec2 ToCanvasCoordinates( const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset, const glm::vec2& coordinates );
ImTextureID GetImTextureID( const std::filesystem::path& filename );
void Update( float delta );

template <typename T>
std::shared_ptr<T> OpenWindow()
{
    std::shared_ptr<T> pWindow = std::make_shared<T>();
    Private::sWindows.push_back( pWindow );
    return pWindow;
}

} // namespace Hyperscape::UI2
