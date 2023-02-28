// Copyright 2018 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include "scenecamera.h"

namespace Genesis
{

SceneCamera::SceneCamera()
    : m_Position(0.0f, 0.0f, -50.0f)
    , m_Target(0.0f, 0.0f, 0.0f)
    , m_Up(0.0f, 1.0f, 0.0f)
{
}

void SceneCamera::CalculateUpVector()
{
    m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

} // namespace Genesis
