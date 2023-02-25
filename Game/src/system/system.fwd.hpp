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

#pragma once

#include <random>

#include <coredefines.h>

namespace Hyperscape
{

// The SystemRandomEngine is used to instantiate astronomical objects.
using SystemRandomEngine = std::mt19937_64;

// The LocalRandomEngine should be seeded from the SystemRandomEngine
// for each detailed astronomical object, so detail on that object
// can be iterated on without affecting the wider system.
using LocalRandomEngine = std::mt19937_64;

GENESIS_DECLARE_SMART_PTR(System);

} // namespace Hyperscape
