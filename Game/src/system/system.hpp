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
#include <random>
#include <string>
#include <vector>

#include <scene/layer.h>

#include "system/system.fwd.hpp"

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(AstronomicalObject);
GENESIS_DECLARE_SMART_PTR(Background);

using AstronomicalObjects = std::vector<AstronomicalObjectUniquePtr>;

class System
{
public:
	System(const std::string& seed);
	~System();

    const std::string& GetSeed() const;
    SystemRandomEngine& GetRandomEngine();

    const AstronomicalObjects& GetAstronomicalObjects() const;

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
    
    void InitializeRandomEngine();
    void InitializeLayers();
    void InitializeBackground();
    void GenerateAstronomicalObjects();
    float GenerateEccentricity();
    int GeneratePlanetCount();
    std::vector<float> GeneratePlanetDistances(int planetCount);
	Genesis::LayerSharedPtr GetLayer(LayerId id) const;

	std::string m_Seed;
    SystemRandomEngine m_RandomEngine;

	std::array<Genesis::LayerSharedPtr, static_cast<size_t>(LayerId::Count)> m_Layers;

	BackgroundUniquePtr m_pBackground;

    AstronomicalObjects m_AstronomicalObjects;
};

}