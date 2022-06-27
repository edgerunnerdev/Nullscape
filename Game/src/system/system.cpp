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

#include <scene/scene.h>
#include <genesis.h>

#include "system/background.hpp"

namespace Hyperscape
{

System::System(const std::string& seed)
    : m_Seed(seed)
{
    InitializeLayers();
    InitializeBackground();
}
   
System::~System()
{
    Genesis::Scene* pScene = Genesis::FrameWork::GetScene();
    for (size_t i = 0; i < m_Layers.size(); ++i)
    {
        pScene->RemoveLayer(m_Layers[i]->GetLayerDepth());
    }
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

Genesis::LayerSharedPtr System::GetLayer(LayerId id) const 
{
    return m_Layers[static_cast<size_t>(id)];
}

} // namespace Hyperscape