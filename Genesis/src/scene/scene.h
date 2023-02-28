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

#pragma once

#include "../taskmanager.h"
#include "coredefines.h"
#include "layer.h"
#include "rendersystem.fwd.h"
#include "scene/light.h"

#include <SDL.h>
#include <glm/vec3.hpp>

#include <array>
#include <list>
#include <memory>
#include <vector>

namespace Genesis
{

GENESIS_DECLARE_SMART_PTR( SceneCamera );

//---------------------------------------------------------------
// Scene
//---------------------------------------------------------------

using LightArray = std::array<Light, 3>;

class Scene : public Task
{
public:
    Scene();
    virtual ~Scene();
    TaskStatus Update( float delta );
    void Render( Viewport* pViewport );
    LayerSharedPtr AddLayer( uint32_t depth, bool isBackground = false );
    void RemoveLayer( uint32_t depth );
    void SetLayerMask( uint32_t mask ); // Allows only layers which match this mask to be rendered
    uint32_t GetLayerMask() const;
    void AddCamera( SceneCameraSharedPtr& pCamera );
    void RemoveCamera( SceneCameraSharedPtr& pCamera );
    const SceneCameraSharedPtr& GetCamera() const; // Gets the first camera.
    const std::vector<SceneCameraSharedPtr>& GetCameras() const;
    const LightArray& GetLights() const;
    LightArray& GetLights();

    template <typename T>
    T* Find()
    {
        for ( auto& pLayer : mLayerList )
        {
            for ( auto& pObject : pLayer->GetSceneObjects() )
            {
                T* pResult = dynamic_cast<T*>( pObject.pSceneObject );
                if ( pResult != nullptr )
                {
                    return pResult;
                }
            }
        }
        return nullptr;
    }

private:
    using LayerList = std::list<LayerSharedPtr>;

    LayerList mLayerList;
    LayerList mLayersToDelete;
    std::vector<SceneCameraSharedPtr> m_Cameras;
    uint32_t mMask;
    LightArray m_Lights;
};
GENESIS_DECLARE_SMART_PTR( Scene );

inline void Scene::AddCamera( SceneCameraSharedPtr& pCamera )
{
    m_Cameras.push_back( pCamera );
}

inline void Scene::RemoveCamera( SceneCameraSharedPtr& pCamera )
{
    m_Cameras.erase( std::remove_if( m_Cameras.begin(), m_Cameras.end(), [ &pCamera ]( SceneCameraSharedPtr& pCam ) { return pCamera == pCam; } ) );
}

inline const SceneCameraSharedPtr& Scene::GetCamera() const
{
    static const SceneCameraSharedPtr sNullCamera;
    return m_Cameras.empty() ? sNullCamera : m_Cameras.front();
}

inline const std::vector<SceneCameraSharedPtr>& Scene::GetCameras() const
{
    return m_Cameras;
}

inline void Scene::SetLayerMask( uint32_t mask )
{
    mMask = mask;
}

inline uint32_t Scene::GetLayerMask() const
{
    return mMask;
}

inline const LightArray& Scene::GetLights() const
{
    return m_Lights;
}
inline LightArray& Scene::GetLights()
{
    return m_Lights;
}

} // namespace Genesis
