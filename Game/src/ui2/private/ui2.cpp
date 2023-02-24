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

#include "ui2/private/ui2.hpp"

#include <array>
#include <filesystem>

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

namespace Nullscape::UI2::Private
{

std::array<ImFont*, static_cast<size_t>( UI2::FontId::Count )> sRegisteredFonts;
std::vector<WindowWeakPtr> sWindows;

void RegisterDefaultFont()
{
    sRegisteredFonts[ static_cast<size_t>( FontId::Default ) ] = ImGui::GetIO().Fonts->AddFontDefault();
}

void RegisterFont( FontId id, const std::filesystem::path& path, float size )
{
    ImFont* pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF( path.generic_string().c_str(), size );
    SDL_assert( pFont );
    sRegisteredFonts[ static_cast<size_t>( id ) ] = pFont;
}

void SetupStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.ChildRounding = 8;
    style.Colors[ ImGuiCol_WindowBg ] = ImColor( 30, 30, 30, 240 );
    style.Colors[ ImGuiCol_ChildBg ] = ImColor( 0, 0, 0, 20 );
    style.Colors[ ImGuiCol_TitleBgActive ] = ImColor( 10, 10, 10, 255 );
    style.Colors[ ImGuiCol_FrameBg ] = ImColor( 50, 50, 50, 140 );
    style.Colors[ ImGuiCol_FrameBgHovered ] = ImColor( 255, 255, 255, 30 );
    style.Colors[ ImGuiCol_FrameBgActive ] = ImColor( 255, 255, 255, 50 );
    style.Colors[ ImGuiCol_SliderGrab ] = ImColor( 86, 224, 199, 255 );
    style.Colors[ ImGuiCol_SliderGrabActive ] = ImColor( 0, 255, 209, 255 );
}

} // namespace Nullscape::UI2::Private
