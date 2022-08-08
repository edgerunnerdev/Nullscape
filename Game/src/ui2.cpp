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
    RegisterFont(FontId::JuraRegular20, "data/ui/fonts/Jura-Regular.ttf", 20.0f);
    RegisterFont(FontId::ArconRegular18, "data/ui/fonts/Arcon-Regular.otf", 18.0f);
    RegisterFont(FontId::ArconBold18, "data/ui/fonts/Arcon-Bold.ttf", 18.0f);

    Genesis::ImGuiImpl::CreateFontsTexture();
    
    SetupStyle();
}

void UI2::RegisterDefaultFont() 
{
    sRegisteredFonts[static_cast<size_t>(FontId::Default)] = ImGui::GetIO().Fonts->AddFontDefault();
}

void UI2::RegisterFont(FontId id, const std::filesystem::path& path, float size)
{
     ImFont* pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.generic_string().c_str(), size);
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

ImVec2 UI2::ToCanvasCoordinates(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset, const glm::vec2& coordinates)
{
    static const float sZoom = 0.5f;
    const ImVec2 size(canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y);
    const float hratio = size.x / size.y;
    return ImVec2(canvasTopLeft.x + canvasOffset.x + size.x * coordinates.x * sZoom / hratio, canvasTopLeft.y + canvasOffset.y + size.y * coordinates.y * sZoom);
}

void UI2::SetupStyle() 
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.ChildRounding = 8;
}

} // namespace Hyperscape
