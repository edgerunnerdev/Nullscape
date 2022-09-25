#include "sceneobject.h"

#include "../rendersystem.h"

namespace Genesis
{

SceneObject::SceneObject()
    : m_Terminating(false)
    , m_RenderHint(RenderHint::None)
    , m_pScene(nullptr)
{
}

SceneObject::~SceneObject() {}

void SceneObject::Update(float delta) {}

void SceneObject::Render() {}
} // namespace Genesis