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
    static void RegisterMenu(const std::string& menuRoot, const std::string& menuName, bool* pShow);
    static void UnregisterMenu(const std::string& menuRoot, const std::string& menuName);

private:
    static void CreateFontsTexture();

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
