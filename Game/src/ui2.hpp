// Copyright 2022 Pedro Nunes
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

#include <filesystem>

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <imgui/imgui.h>
#include <externalheadersend.hpp>
// clang-format on

namespace Nullscape
{

class UI2
{
public:
	enum class FontId
    {
		Default,
		JuraRegular20,
		ArconRegular18,
		ArconBold18,

		Count
	};

	static void Initialize();
	static void PushFont(FontId id);
	static void PopFont();
    static ImVec2 ToCanvasCoordinates(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset, const glm::vec2& coordinates);

private:
	static void RegisterDefaultFont();
	static void RegisterFont(FontId id, const std::filesystem::path& path, float size);
	static void SetupStyle();
};

} // namespace Nullscape
