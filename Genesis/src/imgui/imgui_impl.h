#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <list>
#include <map>
#include <string>

#include "resources/resourceshader.hpp"

namespace Genesis
{

class ResourceImage;
class ShaderUniform;
class VertexBuffer;

class ImGuiImpl
{
public:
    static void Initialise();
    static void Shutdown();
    static void NewFrame(float delta);
    static bool HandleEvent(SDL_Event* pEvent);
    static void Render();
    static bool IsInitialised();
    static bool IsEnabled();
    static void Enable(bool state);
    static bool IsDevMenuEnabled();
    static void EnableDevMenu(bool state);
    static void RegisterDevMenu(const std::string& menuRoot, const std::string& menuName, bool* pShow);
    static void UnregisterDevMenu(const std::string& menuRoot, const std::string& menuName);
    static void CreateFontsTexture();

private:
    static double g_Time;
    static bool g_MousePressed[3];
    static float g_MouseWheel;
    static GLuint g_FontTexture;
    static ResourceShader* m_pDiffuseShader;
    static ShaderUniformSharedPtr m_pDiffuseSampler;
    static ResourceImage* m_pTexture;
    static VertexBuffer* m_pVertexBuffer;
    static bool m_Initialised;
    static bool m_Enabled;
    static bool m_DevMenuEnabled;

    struct Menu
    {
        std::string name;
        bool* pShow;
    };
    typedef std::list<Menu> MenuList;
    typedef std::map<std::string, MenuList> MenuRegistry;
    static MenuRegistry m_MenuRegistry;
};

} // namespace Genesis
