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

#include "ui2/window.hpp"

#include <filesystem>
#include <functional>
#include <vector>

#include <coredefines.h>
#include <imgui/imgui.h>

namespace Nullscape
{

class SidebarWindow : public UI2::Window
{
public:
    SidebarWindow();

    virtual void Update( float delta ) override;

private:
    int m_Width;
    int m_Height;
    int m_IconSize;
    int m_IconSeparation;

    using WindowCreationFn = std::function<UI2::WindowWeakPtr()>;
    struct IconButton
    {
        WindowCreationFn createWindowFn;
        ImTextureID textureId;
        UI2::WindowWeakPtr wpWindow;
    };

    void AddIconButton( const std::filesystem::path& path, WindowCreationFn windowCreationFn );
    std::vector<IconButton> m_IconButtons;
};

GENESIS_DECLARE_SMART_PTR( SidebarWindow );

} // namespace Nullscape
