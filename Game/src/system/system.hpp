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

namespace Nullscape
{

GENESIS_DECLARE_SMART_PTR(AstronomicalObject);
GENESIS_DECLARE_SMART_PTR(Player);
GENESIS_DECLARE_SMART_PTR(Sector);
GENESIS_DECLARE_SMART_PTR(SignalSource);
GENESIS_DECLARE_SMART_PTR(Skybox);

using AstronomicalObjects = std::vector<AstronomicalObjectSharedPtr>;
using SignalSources = std::vector<SignalSourceSharedPtr>;

enum class LayerId
{
    Background = 0,
    Ships,
    Effects,
    Ammo,
    Physics,
    Debug,

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
    const SignalSources& GetSignalSources() const;

    void JumpTo(PlayerSharedPtr pPlayer, const glm::vec2& coordinates);
    Sector* GetCurrentSector();

    Genesis::Layer* GetLayer(LayerId id) const;

private:  
    void InitializeRandomEngine();
    void InitializeLayers();
    void InitializeSkybox();
    void GenerateAstronomicalObjects();
    float GenerateEccentricity();
    int GeneratePlanetCount();
    std::vector<float> GeneratePlanetDistances(int planetCount);
    void GenerateWormholes();

	std::string m_Seed;
    bool m_DemoMode;
    SystemRandomEngine m_RandomEngine;
    std::array<Genesis::LayerSharedPtr, static_cast<size_t>(LayerId::Count)> m_Layers;
	SkyboxUniquePtr m_pSkybox;

    AstronomicalObjects m_AstronomicalObjects;
    SignalSources m_SignalSources;
    SectorUniquePtr m_pCurrentSector;
};

}