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

#include <array>
#include <memory>
#include <string>

#include <scene/layer.h>

namespace Hyperscape
{

class Background;
using BackgroundUniquePtr = std::unique_ptr<Background>;

class System
{
public:
	System(const std::string& seed);
	~System();

private:
    enum class LayerId
    {
        Background = 0,
        Ships,
        Effects,
        Ammo,
        Physics,

        Count
    };

    void InitializeLayers();
    void InitializeBackground();
	Genesis::LayerSharedPtr GetLayer(LayerId id) const;

	std::string m_Seed;

	std::array<Genesis::LayerSharedPtr, static_cast<size_t>(LayerId::Count)> m_Layers;

	BackgroundUniquePtr m_pBackground;
};

}