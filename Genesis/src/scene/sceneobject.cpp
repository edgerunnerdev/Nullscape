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

#include "sceneobject.h"

#include "../rendersystem.h"

namespace Genesis
{

SceneObject::SceneObject()
    : m_Terminating( false )
    , m_RenderHint( RenderHint::None )
    , m_pScene( nullptr )
{
}

SceneObject::~SceneObject() {}

void SceneObject::Update( float delta ) {}

} // namespace Genesis