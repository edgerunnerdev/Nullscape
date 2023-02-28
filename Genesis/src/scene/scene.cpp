#include "scene.h"

#include "layer.h"
#include "rendersystem.fwd.h"
#include "genesis.h"
#include "taskmanager.h"

namespace Genesis
{

//---------------------------------------------------------------
// Scene
//---------------------------------------------------------------

Scene::Scene()
{
    mMask = 0xFFFFFFFF;
    FrameWork::GetTaskManager()->AddTask("Scene", this, (TaskFunc)&Scene::Update, TaskPriority::GameLogic);
}

Scene::~Scene()
{
    if (FrameWork::GetTaskManager())
    {
        FrameWork::GetTaskManager()->RemoveTask(this);
    }
}

TaskStatus Scene::Update(float delta)
{
    for (auto& pLayer : mLayerList)
    {
        pLayer->Update(delta);
    }

    return TaskStatus::Continue;
}

void Scene::Render(Viewport* pViewport)
{
    for (auto& pLayer : mLayerList)
    {
        if (pLayer->GetLayerDepth() & GetLayerMask())
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            pLayer->Render(pViewport);
        }
    }

    for (auto& pLayerToDelete : mLayersToDelete)
    {
        for (LayerList::iterator it = mLayerList.begin(), itEnd = mLayerList.end(); it != itEnd;)
        {
            if (*it == pLayerToDelete)
            {
                it = mLayerList.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
    mLayersToDelete.clear();
}

LayerSharedPtr Scene::AddLayer(uint32_t depth, bool isBackground /* = false */)
{
    for (auto& pLayer : mLayerList)
    {
        if (pLayer->GetLayerDepth() == depth && pLayer->IsBackground() == isBackground && pLayer->IsMarkedForDeletion() == false)
        {
            return pLayer;
        }
    }

    LayerSharedPtr pLayer = std::make_shared<Layer>(depth, isBackground);
    pLayer->SetScene(this);
    mLayerList.push_back(pLayer);
    return pLayer;
}

void Scene::RemoveLayer(uint32_t depth)
{
    for (auto& pLayer : mLayerList)
    {
        if (pLayer->GetLayerDepth() == depth)
        {
            mLayersToDelete.push_back(pLayer);
            pLayer->MarkForDeletion();
        }
    }
}

} // namespace Genesis