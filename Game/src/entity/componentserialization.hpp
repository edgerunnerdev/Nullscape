// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
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

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/ext/pointer.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/std_smart_ptr.h>
#include <externalheadersend.hpp>
// clang-format on

#include "entity/components/modelcomponent.hpp"
#include "entity/components/transformcomponent.hpp"
#include "entity/component.hpp"

namespace Hyperscape
{

using ComponentPolymorphicClasses = bitsery::ext::PolymorphicClassesList<Component>;

} // namespace Hyperscape

namespace bitsery::ext
{

// for each base class define DIRECTLY derived classes
// e.g. PolymorphicBaseClass<Shape> : PolymorphicDerivedClasses<Circle, Rectangle, RoundedRectangle>
// is incorrect, because RoundedRectangle does not directly derive from Shape
template<> struct PolymorphicBaseClass<Hyperscape::Component> : PolymorphicDerivedClasses<
	Hyperscape::ModelComponent,
	Hyperscape::TransformComponent
>
{
};

} // namespace bitsery::ext
