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

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <scene/layer.h>

#include "system/system.fwd.hpp"

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR(AstronomicalObject);
GENESIS_DECLARE_SMART_PTR(Background);
GENESIS_DECLARE_SMART_PTR(Player);
GENESIS_DECLARE_SMART_PTR(Sector);

using AstronomicalObjects = std::vector<AstronomicalObjectUniquePtr>;

enum class LayerId
{
    Background = 0,
    Ships,
    Effects,
    Ammo,
    Physics,

    Count
};

class System
{
public:
	System(const std::string& seed, bool demoMode = false);
	~System();

    void Update(float delta);

    const std::string& GetSeed() const;
    SystemRandomEngine& GetRandomEngine();

    const AstronomicalObjects& GetAstronomicalObjects() const;
    glm::ivec2 GetNumSectors() const;

    void JumpTo(PlayerSharedPtr pPlayer, const glm::ivec2& coordinates);
    Sector* GetCurrentSector();

    Genesis::Layer* GetLayer(LayerId id) const;

private:  
    void InitializeRandomEngine();
    void InitializeLayers();
    void InitializeBackground();
    void GenerateAstronomicalObjects();
    float GenerateEccentricity();
    int GeneratePlanetCount();
    std::vector<float> GeneratePlanetDistances(int planetCount);

	std::string m_Seed;
    bool m_DemoMode;
    SystemRandomEngine m_RandomEngine;
    std::array<Genesis::LayerSharedPtr, static_cast<size_t>(LayerId::Count)> m_Layers;
	BackgroundUniquePtr m_pBackground;

    AstronomicalObjects m_AstronomicalObjects;

    static const int sSectorsX = 31;
    static const int sSectorsY = 31;
    SectorUniquePtr m_pCurrentSector;
};

}