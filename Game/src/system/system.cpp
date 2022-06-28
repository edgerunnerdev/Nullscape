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

#include "system/astronomicalobject/star.hpp"
#include "system/background.hpp"

namespace Hyperscape
{

System::System(const std::string& seed)
    : m_Seed(seed)
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
}

Genesis::LayerSharedPtr System::GetLayer(LayerId id) const 
{
    return m_Layers[static_cast<size_t>(id)];
}

} // namespace Hyperscape