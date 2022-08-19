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

#include <coredefines.h>

namespace Nullscape
{

GENESIS_DECLARE_SMART_PTR(System);

class SystemViewer
{
public:
    SystemViewer();
    ~SystemViewer();

    void UpdateDebugUI();

    void View(SystemSharedPtr pSystem);

private:
    void Reseed(int depth);
    void DrawCanvas();

    SystemWeakPtr m_pSystem;
    bool m_IsOpen;

    // The reseed system is used only for debugging purposes, allowing us to quickly iterate.
    SystemSharedPtr m_pReseedSystem;
};

} // namespace Nullscape