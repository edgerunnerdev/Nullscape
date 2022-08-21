// Copyright 2015 Pedro Nunes
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

#include <glm/vec2.hpp>

namespace Nullscape
{

class SectorCamera
{
public:
    SectorCamera();
    ~SectorCamera();

    void Update(float delta);
    void GetBorders(glm::vec2& topLeft, glm::vec2& topRight) const;

private:
    void UpdateListener(float delta);
    void UpdateBorders();

    glm::vec2 m_BorderTopLeft;
    glm::vec2 m_BorderBottomRight;
};

} // namespace Nullscape
