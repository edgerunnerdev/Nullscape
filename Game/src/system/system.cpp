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

#include "system/system.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <xxhash64.h>
#include <externalheadersend.hpp>
// clang-format on

#include <scene/scene.h>
#include <genesis.h>

#include "sector/sectorinfo.h"
#include "sector/sector.h"
#include "system/astronomicalobject/orbit.hpp"
#include "system/astronomicalobject/planet.hpp"
#include "system/astronomicalobject/star.hpp"
#include "system/background.hpp"

namespace Hyperscape
{

System::System(const std::string& seed, bool demoMode /* = false*/)
    : m_Seed(seed)
    , m_DemoMode(demoMode)
{
    InitializeRandomEngine();
    InitializeLayers();
    InitializeBackground();
    GenerateAstronomicalObjects();
}
   
System::~System()
{
    Genesis::Scene* pScene = Genesis::FrameWork::GetScene();
    for (size_t i = 0; i < m_Layers.size(); ++i)
    {
        pScene->RemoveLayer(m_Layers[i]->GetLayerDepth());
    }
}

void System::Update(float delta) 
{
    if (m_pCurrentSector)
    {
        m_pCurrentSector->Update(delta);
    }
}

const std::string& System::GetSeed() const 
{
    return m_Seed;
}

SystemRandomEngine& System::GetRandomEngine() 
{
    return m_RandomEngine;
}

const AstronomicalObjects& System::GetAstronomicalObjects() const 
{
    return m_AstronomicalObjects;
}

void System::JumpTo(PlayerSharedPtr pPlayer, const glm::vec2& coordinates) 
{
    if (m_pCurrentSector != nullptr)
    {
        SDL_assert(false); // TODO
    }

    m_pCurrentSector = std::make_unique<Sector>(this, coordinates);
    bool res = m_pCurrentSector->Initialize();
    SDL_assert(res);
}

Sector* System::GetCurrentSector() 
{
    return m_pCurrentSector.get();
}

void System::InitializeRandomEngine() 
{
    uint64_t seedHash = XXHash64::hash(m_Seed.data(), m_Seed.length(), 0);
    m_RandomEngine = SystemRandomEngine(seedHash);
}

void System::InitializeLayers()
{
    Genesis::Scene* pScene = Genesis::FrameWork::GetScene();
    m_Layers[static_cast<size_t>(LayerId::Background)] = pScene->AddLayer(1, true);
    m_Layers[static_cast<size_t>(LayerId::Ships)] = pScene->AddLayer(2);
    m_Layers[static_cast<size_t>(LayerId::Effects)] = pScene->AddLayer(3);
    m_Layers[static_cast<size_t>(LayerId::Ammo)] = pScene->AddLayer(4);
    m_Layers[static_cast<size_t>(LayerId::Physics)] = pScene->AddLayer(5);
}

void System::InitializeBackground() 
{
    m_pBackground = std::make_unique<Background>(m_Seed);
    GetLayer(LayerId::Background)->AddSceneObject(m_pBackground.get(), false);
}

void System::GenerateAstronomicalObjects() 
{
    StarUniquePtr pStar = std::make_unique<Star>(GetRandomEngine(), glm::vec2(0.0f, 0.0f));
    m_AstronomicalObjects.push_back(std::move(pStar));

    std::uniform_real_distribution<float> orbitDistribution(0.0f, 360.0f);
    int planetCount = GeneratePlanetCount();
    std::vector<float> planetDistances = GeneratePlanetDistances(planetCount);

    for (int i = 0; i < planetCount; ++i)
    {
        OrbitUniquePtr pOrbit = std::make_unique<Orbit>(planetDistances[i], GenerateEccentricity());
        PlanetUniquePtr pPlanet = std::make_unique<Planet>(GetRandomEngine(), std::move(pOrbit), orbitDistribution(GetRandomEngine()));
        m_AstronomicalObjects.push_back(std::move(pPlanet));
    }
}

// In the solar system, all planets have an eccentricity between 0.006 (Venus) and 0.248 (Pluto).
// We generate eccentricity values between [0-0.25], but greatly weight the results towards the lower end.
float System::GenerateEccentricity() 
{
    const std::uniform_real_distribution<float> eccentricityDistribution(0.0f, 1.0);
    const float v = eccentricityDistribution(GetRandomEngine());
    const float fv = glm::clamp(-log10(-v+1.0f)/2.0f, 0.0f, 1.0f) * 0.25f;
    return fv;
}

int System::GeneratePlanetCount() 
{
    // Completely artificial set of values as we currently don't have enough data to generate a good table.
    // See https://en.wikipedia.org/wiki/List_of_multiplanetary_systems
    // Either way, we do want most systems to have some planets.
    const std::array<int, 9> count = { 100, 500, 700, 1000, 300, 200, 100, 50, 10 };
    int accum = 0;
    for (int i = 0; i < count.size(); ++i)
    {
        accum += count[i];
    }

    const std::uniform_int_distribution<int> planetDistribution(0, accum);
    const int v = planetDistribution(GetRandomEngine());
    accum = 0;
    for (int i = 0; i < count.size(); ++i)
    {
        accum += count[i];
        if (v <= accum)
        {
            return i;
        }
    }
    return 0;
}

std::vector<float> System::GeneratePlanetDistances(int planetCount) 
{
    std::vector<float> distances;
    distances.reserve(planetCount);

    const std::uniform_real_distribution<float> distanceDistribution(0.2f, 0.8f);
    float accumulatedDistance = 0.0f;
    for (int i = 0; i < planetCount; ++i)
    {
        accumulatedDistance += distanceDistribution(GetRandomEngine());
        distances.push_back(accumulatedDistance);
    }

    if (accumulatedDistance > 1.0f)
    {
        for (int i = 0; i < planetCount; ++i)
        {
            distances[i] /= accumulatedDistance;
        }
    }

    for (int i = 0; i < planetCount; ++i)
    {
        distances[i] *= 0.75f;
    }

    return distances;
}

Genesis::Layer* System::GetLayer(LayerId id) const
{
    return m_Layers[static_cast<size_t>(id)].get();
}

} // namespace Hyperscape