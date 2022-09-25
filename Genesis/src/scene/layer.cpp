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

#include "layer.h"

#include "render/viewport.hpp"
#include "../genesis.h"
#include "../rendersystem.h"
#include "sceneobject.h"

#include <log.hpp>

namespace Genesis
{

Layer::Layer(uint32_t depth, bool isBackground)
    : mDepth(depth)
    , mIsBackground(isBackground)
    , mMarkedForDeletion(false)
    , m_pScene(nullptr)
{
}

Layer::~Layer()
{
    // Remove any remaining objects from the layer
    LayerObjectList::iterator itEnd = mObjectList.end();
    for (LayerObjectList::iterator it = mObjectList.begin(); it != itEnd; ++it)
    {
        if (it->hasOwnership)
        {
            delete it->pSceneObject;
        }
    }
}

// Update all objects in this layer
void Layer::Update(float delta)
{
    if (IsMarkedForDeletion())
        return;

    {
        LayerObjectList::const_iterator itEnd = mObjectList.end();
        for (LayerObjectList::const_iterator it = mObjectList.begin(); it != itEnd; ++it)
        {
            it->pSceneObject->Update(delta);
        }
    }

    if (mToRemove.empty() == false)
    {
        SceneObjectList::iterator itEnd = mToRemove.end();
        for (SceneObjectList::iterator it = mToRemove.begin(); it != itEnd; ++it)
        {
            LayerObjectList::iterator it2End = mObjectList.end();
            for (LayerObjectList::iterator it2 = mObjectList.begin(); it2 != it2End; ++it2)
            {
                if (it2->pSceneObject == *it)
                {
                    it2->pSceneObject->SetScene(nullptr);
                    if (it2->hasOwnership)
                    {
                        delete *it;
                    }
                    mObjectList.erase(it2);
                    break;
                }
            }
        }
        mToRemove.clear();
    }
}

// Render all objects in this layer
void Layer::Render(Viewport* pViewport)
{
    if (IsMarkedForDeletion())
        return;

    RenderSystem* pRenderSystem = FrameWork::GetRenderSystem();

    Scene* pScene = pViewport ? pViewport->GetScene() : nullptr;
    int width = pViewport ? pViewport->GetWidth() : 0;
    int height = pViewport ? pViewport->GetHeight() : 0;

    if (IsBackground())
    {
        pRenderSystem->ViewOrtho(width, height);
    }
    else
    {
        pRenderSystem->ViewPerspective(width, height, pScene);
    }

    LayerObjectList::const_iterator itEnd = mObjectList.end();
    for (LayerObjectList::const_iterator it = mObjectList.begin(); it != itEnd; ++it)
    {
        if (it->pSceneObject->IsTerminating() == false)
        {
            it->pSceneObject->Render();
        }
    }
}

void Layer::AddSceneObject(SceneObject* pObject, bool hasOwnership /* = true */)
{
    SDL_assert(pObject != nullptr);

#ifdef _DEBUG
    for (auto& pLayerObject : mObjectList)
    {
        if (pLayerObject.pSceneObject == pObject)
        {
            Log::Warning() << "Object has already been added to the layer!";
        }
    }
#endif

    LayerObject obj;
    obj.pSceneObject = pObject;
    obj.pSceneObject->SetScene(GetScene());
    obj.hasOwnership = hasOwnership;
    if (pObject->GetRenderHint() == RenderHint::Transparent)
    {
        mObjectList.push_back(obj);
    }
    else 
    {
        mObjectList.push_front(obj);
    }
}

// We can't remove the objects immediately, since they may be
// being stepped by the Update. Therefore we only remove them
// when the update is finished.
void Layer::RemoveSceneObject(SceneObject* object)
{
    object->SetTerminating();
    mToRemove.push_back(object);
}
} // namespace Genesis