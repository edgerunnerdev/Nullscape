// Copyright 2021 Pedro Nunes
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

#include "ui/panel.h"

namespace Nullscape::UI
{

class Meter : public Panel
{
public:
    Meter( const std::string& name, int maxValue );

    virtual void RenderProperties() override;

protected:
    virtual void SaveProperties( json& properties ) override;
    virtual void LoadProperties( const json& properties ) override;

private:
    void UpdatePips();

    using PanelVector = std::vector<Genesis::Gui::Panel*>;
    PanelVector m_Panels;
    int m_Pips;
};

} // namespace Nullscape::UI
