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

#include "ui2.hpp"

#include <array>

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

namespace Hyperscape
{

static std::array<ImFont*, static_cast<size_t>(UI2::FontId::Count)> sRegisteredFonts;

void UI2::Initialize()
{
    for (size_t i = 0; i < sRegisteredFonts.size(); ++i)
    {
        sRegisteredFonts[i] = nullptr;
    }

    RegisterDefaultFont();
    RegisterFont(FontId::JuraRegular16, "data/ui/fonts/Jura-Regular.ttf", 16.0f);

    Genesis::ImGuiImpl::CreateFontsTexture();
    
    SetupStyle();
}

void UI2::RegisterDefaultFont() 
{
    sRegisteredFonts[static_cast<size_t>(FontId::Default)] = ImGui::GetIO().Fonts->AddFontDefault();
}

void UI2::RegisterFont(FontId id, const std::filesystem::path& path, float size)
{
    ImFontConfig fontConfig = ImFontConfig();
    fontConfig.OversampleH = 6;
    fontConfig.OversampleV = 6;

     ImFont* pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.generic_string().c_str(), size, &fontConfig);
     SDL_assert(pFont);
     sRegisteredFonts[static_cast<size_t>(id)] = pFont;
}

void UI2::PushFont(FontId id)
{
    ImFont* pFont = sRegisteredFonts[static_cast<size_t>(id)];
    SDL_assert(pFont);
    ImGui::PushFont(pFont);
}

void UI2::PopFont()
{
	ImGui::PopFont();
}

void UI2::SetupStyle() 
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.ChildRounding = 8;
}

} // namespace Hyperscape
