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

#include "windows/sidebarwindow.hpp"

#include <configuration.h>
#include <genesis.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

#include "ui2/ui2.hpp"

namespace Nullscape
{

SidebarWindow::SidebarWindow()
    : Window( "Sidebar" )
{
    m_Width = 48;
    m_Height = Genesis::Configuration::GetScreenHeight();
    m_IconSize = 24;
    m_IconSeparation = 16;

    AddIconButton( "data/ui/icons/sensors.png", nullptr );
}

void SidebarWindow::Update( float delta )
{
    using namespace ImGui;

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoBackground;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
    SetNextWindowSize( ImVec2( m_Width, m_Height ), ImGuiCond_Always );
    Begin( "Sidebar", nullptr, flags );

    ImDrawList* pDrawList = GetWindowDrawList();

    pDrawList->PushClipRectFullScreen();
    pDrawList->AddRectFilled( ImVec2( 0, 0 ), ImVec2( m_Width, m_Height ), IM_COL32( 0, 0, 0, 80 ) );

    pDrawList->AddImage(
        UI2::GetImTextureID( "data/ui/portraits/portrait_f_001.png" ),
        ImVec2( 0, 0 ), ImVec2( 48, 48 ) );

    int iconOffsetY = 48 + m_IconSeparation;
    for ( auto& iconButton : m_IconButtons )
    {
        bool isHovered = false;
        ImU32 color = isHovered ? IM_COL32( 240, 240, 240, 255 ) : IM_COL32( 200, 200, 200, 255 );
        ImVec2 pos( ( m_Width - m_IconSize ) / 2, iconOffsetY );
        pDrawList->AddImage( iconButton.textureId, pos, ImVec2( pos.x + m_IconSize, pos.y + m_IconSize ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), color );
        iconOffsetY += m_IconSize;
    }

    pDrawList->PopClipRect();

    End();
}

void SidebarWindow::AddIconButton( const std::filesystem::path& path, WindowCreationFn windowCreationFn )
{
    IconButton iconButton;
    iconButton.textureId = UI2::GetImTextureID( path );
    iconButton.createWindowFn = windowCreationFn;
    m_IconButtons.push_back( iconButton );
}

} // namespace Nullscape
