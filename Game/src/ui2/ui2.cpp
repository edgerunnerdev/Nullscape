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

#include "ui2/ui2.hpp"

#include <SDL.h>
#include <array>
#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <resources/resourceimage.h>
#include <genesis.h>
#include <resourcemanager.h>

#include <ui2/window.hpp>

namespace Hyperscape::UI2
{

void Initialize()
{
    using namespace Private;

    for ( size_t i = 0; i < sRegisteredFonts.size(); ++i )
    {
        sRegisteredFonts[ i ] = nullptr;
    }

    RegisterDefaultFont();
    RegisterFont( FontId::JuraRegular20, "data/ui/fonts/Jura-Regular.ttf", 20.0f );
    RegisterFont( FontId::ArconRegular18, "data/ui/fonts/Arcon-Regular.otf", 18.0f );
    RegisterFont( FontId::ArconBold18, "data/ui/fonts/Arcon-Bold.ttf", 18.0f );

    Genesis::ImGuiImpl::CreateFontsTexture();

    SetupStyle();
}

void PushFont( FontId id )
{
    ImFont* pFont = Private::sRegisteredFonts[ static_cast<size_t>( id ) ];
    SDL_assert( pFont );
    ImGui::PushFont( pFont );
}

void PopFont()
{
    ImGui::PopFont();
}

ImVec2 ToCanvasCoordinates( const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset, const glm::vec2& coordinates )
{
    static const float sZoom = 0.5f;
    const ImVec2 size( canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y );
    const float hratio = size.x / size.y;
    return ImVec2( canvasTopLeft.x + canvasOffset.x + size.x * coordinates.x * sZoom / hratio, canvasTopLeft.y + canvasOffset.y + size.y * coordinates.y * sZoom );
}

ImTextureID GetImTextureID( const std::filesystem::path& filename )
{
    using namespace Genesis;
    ResourceImage* pResourceImage = FrameWork::GetResourceManager()->GetResource<ResourceImage*>(filename.generic_string());
    if (pResourceImage)
    {
        return reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(pResourceImage->GetTexture()));
    }
    else
    {
        return 0;
    }
}

void Update( float delta )
{
    using namespace Private;
    for ( WindowWeakPtr& wpWindow : sWindows )
    {
        WindowSharedPtr pWindow = wpWindow.lock();
        if ( pWindow )
        {
            pWindow->Update( delta );
        }
    }

    sWindows.erase( std::remove_if( sWindows.begin(), sWindows.end(), []( WindowWeakPtr pWindow ) { return pWindow.expired(); } ), sWindows.end() );
}

} // namespace Hyperscape::UI2
