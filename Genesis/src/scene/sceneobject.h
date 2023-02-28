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

#include "coredefines.h"

namespace Genesis
{

GENESIS_DECLARE_SMART_PTR( Scene );
GENESIS_DECLARE_SMART_PTR( SceneCamera );

enum class RenderHint
{
    None = 0,
    Transparent
};

class SceneObject
{
public:
    SceneObject();
    virtual ~SceneObject();
    virtual void Update( float delta );
    virtual void Render( const SceneCameraSharedPtr& pCamera ) = 0;

    void SetTerminating();
    bool IsTerminating() const;

    RenderHint GetRenderHint() const;
    void SetRenderHint( RenderHint renderHint );

    void SetScene( Scene* pScene );
    Scene* GetScene() const;

    virtual void OnAddedToScene( Scene* pScene ) {}
    virtual void OnRemovedFromScene() {}

private:
    bool m_Terminating;
    RenderHint m_RenderHint;
    Scene* m_pScene;
};

inline void SceneObject::SetTerminating()
{
    m_Terminating = true;
}
inline bool SceneObject::IsTerminating() const
{
    return m_Terminating;
}

inline RenderHint SceneObject::GetRenderHint() const
{
    return m_RenderHint;
}

inline void SceneObject::SetRenderHint( RenderHint renderHint )
{
    m_RenderHint = renderHint;
}

inline void SceneObject::SetScene( Scene* pScene )
{
    if ( m_pScene != nullptr && pScene == nullptr )
    {
        m_pScene = nullptr;
        OnRemovedFromScene();
    }
    else if ( m_pScene != pScene )
    {
        m_pScene = pScene;
        OnAddedToScene( pScene );
    }
}

inline Scene* SceneObject::GetScene() const
{
    return m_pScene;
}

} // namespace Genesis
