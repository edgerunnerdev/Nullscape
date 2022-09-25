#pragma once

namespace Genesis
{

class Scene;

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
    virtual void Update(float);
    virtual void Render() = 0;

    void SetTerminating();
    bool IsTerminating() const;

    RenderHint GetRenderHint() const;
    void SetRenderHint(RenderHint renderHint);

    void SetScene(Scene* pScene);
    Scene* GetScene() const;

    virtual void OnAddedToScene(Scene* pScene) {}
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

inline void SceneObject::SetRenderHint(RenderHint renderHint) 
{
    m_RenderHint = renderHint;
}

inline void SceneObject::SetScene(Scene* pScene)
{
    if (m_pScene != nullptr && pScene == nullptr)
    {
        m_pScene = nullptr;
        OnRemovedFromScene();
    }
    else if (m_pScene != pScene)
    {
        m_pScene = pScene;
        OnAddedToScene(pScene);
    }
}

inline Scene* SceneObject::GetScene() const
{
    return m_pScene;
}

} // namespace Genesis
