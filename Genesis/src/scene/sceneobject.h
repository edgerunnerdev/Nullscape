#pragma once

namespace Genesis
{

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

private:
    bool m_Terminating;
    RenderHint m_RenderHint;
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

} // namespace Genesis
