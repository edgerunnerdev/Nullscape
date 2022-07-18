// Copyright 2022 Pedro Nunes
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

namespace Hyperscape
{

class UI2
{
public:
	enum class FontId
    {
		Default,
		JuraRegular16,

		Count
	};

	static void Initialize();
	static void PushFont(FontId id);
	static void PopFont();

private:
	static void RegisterDefaultFont();
	static void RegisterFont(FontId id, const std::filesystem::path& path, float size);
	static void SetupStyle();
};

} // namespace Hyperscape
