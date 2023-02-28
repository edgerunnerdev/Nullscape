// Copyright 2016 Pedro Nunes
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

#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "configuration.h"
#include "genesis.h"
#include "gui/gui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl.h"
#include "memory.h"
#include "render/gldebugmessagecallback.h"
#include "render/rendertarget.h"
#include "render/viewport.hpp"
#include "rendersystem.fwd.h"
#include "rendersystem.h"
#include "resourcemanager.h"
#include "resources/resourceimage.h"
#include "resources/resourcemodel.h"
#include "scene/scene.h"
#include "scene/scenecamera.h"
#include "shaderuniform.h"
#include "window.h"

#include <SDL_image.h>
#include <log.hpp>

// These exports tell the drivers to use a high performance GPU.
// Without these and in computers which have an integrated graphics card, it is
// possible the game will attempt to use it rather than the dedicated GPU.
#ifdef _WIN32
extern "C" {
__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
__declspec( dllexport ) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

namespace Genesis
{

RenderSystem::RenderSystem()
    : m_ScreenshotScheduled( false )
    , m_CaptureInProgress( false )
    , m_ScreenWidth( 0 )
    , m_ScreenHeight( 0 )
    , m_pPostProcessShader( nullptr )
    , m_pPostProcessVertexBuffer( nullptr )
    , m_pGlowShader( nullptr )
    , m_pGlowShaderSampler( nullptr )
    , m_pGlowShaderDirection( nullptr )
    , m_pGlowVertexBuffer( nullptr )
    , m_ShaderTimer( 0.0f )
    , m_DrawCallCount( 0 )
    , m_BlendMode( BlendMode::Disabled )
    , m_InputCallbackScreenshot( InputManager::sInvalidInputCallbackToken )
    , m_InputCallbackCapture( InputManager::sInvalidInputCallbackToken )
    , m_DebugWindowOpen( false )
{
    // Don't do any OpenGL operations here
    // OpenGL is only available when RenderSystem::Initialize() is called

    for ( size_t i = 0; i < m_PostProcessShaderUniforms.size(); ++i )
    {
        m_PostProcessShaderUniforms[ i ] = nullptr;
    }
}

RenderSystem::~RenderSystem()
{
    ImGuiImpl::UnregisterDevMenu( "Engine", "Render system" );

    InputManager* pInputManager = FrameWork::GetInputManager();
    if ( pInputManager != nullptr )
    {
        pInputManager->RemoveKeyboardCallback( m_InputCallbackScreenshot );
        pInputManager->RemoveKeyboardCallback( m_InputCallbackCapture );
    }

    if ( m_pPostProcessShader )
    {
        m_pPostProcessShader->UnregisterForgeRebuildCallback( this );
    }

    delete m_pPostProcessVertexBuffer;

    ImGuiImpl::Shutdown();
}

void RenderSystem::Initialize( GLuint screenWidth, GLuint screenHeight )
{
    GLenum err = glewInit();
    if ( err == GLEW_OK )
    {
        Log::Info() << "GLEW initialised.";
    }
    else
    {
        Log::Error() << "GLEW initialisation failed: " << glewGetErrorString( err );
        return;
    }

#if OPENGL_ERROR_CHECKING
    // Enables OpenGL warnings.
    // Ignored warning levels / messages can be configured in render/gldebugmessagecallback.cpp.
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    glDebugMessageCallback( &GLDebugMessageCallback, nullptr );
#endif

    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    // Initialize everything that might be needed for debugging purposes
    InitializeDebug();

    // Log graphics card information
    Log::Info() << "Graphics card info:";
    Log::Info() << " - Vendor: " << glGetString( GL_VENDOR );
    Log::Info() << " - Renderer: " << glGetString( GL_RENDERER );
    Log::Info() << " - OpenGL version: " << glGetString( GL_VERSION );
    Log::Info() << " - Shader model info: " << glGetString( GL_SHADING_LANGUAGE_VERSION );

    int major, minor;
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major );
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &minor );
    Log::Info() << "Using OpenGL version " << major << "." << minor;

    m_ProjectionMatrix = glm::perspective( 45.0f, static_cast<float>( m_ScreenWidth ) / static_cast<float>( m_ScreenHeight ), 1.0f, 1000.0f );
    m_ViewMatrix = glm::mat4( 1.0f );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepth( 1.0f );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );

    m_pScene = std::make_shared<Scene>();
    m_pCamera = std::make_shared<SceneCamera>();
    m_pScene->AddCamera( m_pCamera );
    m_pPrimaryViewport = std::make_shared<Viewport>( "Primary viewport", m_ScreenWidth, m_ScreenHeight, m_pScene, m_pCamera );
    AddViewport( m_pPrimaryViewport );

    CreateRenderTargets();
    InitializePostProcessing();
    InitializeGlowChain();

    m_InputCallbackScreenshot = FrameWork::GetInputManager()->AddKeyboardCallback( std::bind( &RenderSystem::TakeScreenshot, this ), SDL_SCANCODE_PRINTSCREEN, ButtonState::Pressed );
    m_InputCallbackCapture = FrameWork::GetInputManager()->AddKeyboardCallback( std::bind( &RenderSystem::Capture, this ), SDL_SCANCODE_F8, ButtonState::Pressed );
}

void RenderSystem::CreateRenderTargets()
{
    // The glow effect is done in four stages:
    // 1) The desired geometry is rendered into the Glow render target during the normal scene render.
    // 2) The Glow render target is then used as source to create a horizontal blur.
    // 3) The Horizontal blur render target is then used as source to create a vertical blur.
    // 4) This final Vertical blur render target is composited onto the main scene as part of the post-processing shader.
    const GLuint glowWidth = m_ScreenWidth / 2;
    const GLuint glowHeight = m_ScreenHeight / 2;

    m_GlowRenderTarget = CreateRenderTarget( "Glow", glowWidth, glowHeight, false, false, true );
    m_GlowHorizontalBlurRenderTarget = CreateRenderTarget( "Glow blur (horizontal)", glowWidth, glowHeight, false, false, true );
    m_GlowVerticalBlurRenderTarget = CreateRenderTarget( "Glow blur (vertical)", glowWidth, glowHeight, false, false, true );
}

RenderTargetSharedPtr RenderSystem::CreateRenderTarget( const std::string& name, GLuint width, GLuint height, bool hasDepth, bool hasStencil, bool autoClear )
{
    RenderTargetSharedPtr pRenderTarget = RenderTarget::Create( name, width, height, hasDepth, hasStencil, autoClear );
    m_RenderTargets.push_back( pRenderTarget );
    return pRenderTarget;
}

RenderTargetSharedPtr RenderSystem::GetRenderTarget( const std::string& name ) const
{
    for ( auto& pRenderTarget : m_RenderTargets )
    {
        if ( pRenderTarget->GetName() == name )
        {
            return pRenderTarget;
        }
    }

    return nullptr;
}

// Clears all render targets and sets the renderer back to a known state.
void RenderSystem::ClearAll()
{
    ResetDrawCallCount();
    glScissor( 0, 0, Configuration::GetScreenWidth(), Configuration::GetScreenHeight() );

    for ( auto& pRenderTarget : m_RenderTargets )
    {
        if ( pRenderTarget->GetAutoClear() )
        {
            pRenderTarget->Clear();
        }
    }

    SetBlendMode( BlendMode::Disabled );
    SetRenderTarget( m_pPrimaryViewport->GetRenderTarget() );
}

void RenderSystem::DrawDebugWindow()
{
    if ( m_DebugWindowOpen == false )
    {
        return;
    }

    ImGui::SetNextWindowSize( ImVec2( 600, 400 ), ImGuiCond_Appearing );
    ImGui::Begin( "Rendering", &m_DebugWindowOpen );

    if ( ImGui::CollapsingHeader( "Render targets", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if ( ImGui::BeginTable( "RenderTargetsTable", 4, flags ) )
        {
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed );
            ImGui::TableSetupColumn( "Width", ImGuiTableColumnFlags_WidthFixed );
            ImGui::TableSetupColumn( "Height", ImGuiTableColumnFlags_WidthFixed );
            ImGui::TableSetupColumn( "Image (half size)" );
            ImGui::TableHeadersRow();

            auto drawRenderTargetFn = []( RenderTarget* pRenderTarget ) {
                ImGui::TableNextColumn();
                ImGui::Text( "%s", pRenderTarget->GetName().c_str() );
                ImGui::TableNextColumn();
                ImGui::Text( "%d", pRenderTarget->GetWidth() );
                ImGui::TableNextColumn();
                ImGui::Text( "%d", pRenderTarget->GetHeight() );
                ImGui::TableNextColumn();
                ImGui::Image( reinterpret_cast<ImTextureID>( static_cast<uintptr_t>( pRenderTarget->GetColor() ) ),
                    ImVec2( static_cast<float>( pRenderTarget->GetWidth() ) / 2.0f, static_cast<float>( pRenderTarget->GetHeight() ) / 2.0f ), ImVec2( 0, 0 ), // UV1
                    ImVec2( 1, 1 ), // UV2
                    ImVec4( 1, 1, 1, 1 ), // Tint
                    ImVec4( 1, 1, 1, 1 ) // Border
                );
            };

            for ( auto& pRenderTarget : m_RenderTargets )
            {
                drawRenderTargetFn( pRenderTarget.get() );
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void RenderSystem::InitializeDebug()
{
    ImGuiImpl::RegisterDevMenu( "Engine", "Render system", &m_DebugWindowOpen );

    mInternalFormatMap[ GL_STENCIL_INDEX ] = "GL_STENCIL_INDEX";
    mInternalFormatMap[ GL_DEPTH_COMPONENT ] = "GL_DEPTH_COMPONENT";
    mInternalFormatMap[ GL_ALPHA ] = "GL_ALPHA";
    mInternalFormatMap[ GL_RGB ] = "GL_RGB";
    mInternalFormatMap[ GL_RGBA ] = "GL_RGBA";
    mInternalFormatMap[ GL_LUMINANCE ] = "GL_LUMINANCE";
    mInternalFormatMap[ GL_LUMINANCE_ALPHA ] = "GL_LUMINANCE_ALPHA";
    mInternalFormatMap[ GL_ALPHA4 ] = "GL_ALPHA4";
    mInternalFormatMap[ GL_ALPHA8 ] = "GL_ALPHA8";
    mInternalFormatMap[ GL_ALPHA12 ] = "GL_ALPHA12";
    mInternalFormatMap[ GL_ALPHA16 ] = "GL_ALPHA16";
    mInternalFormatMap[ GL_LUMINANCE4 ] = "GL_LUMINANCE4";
    mInternalFormatMap[ GL_LUMINANCE8 ] = "GL_LUMINANCE8";
    mInternalFormatMap[ GL_LUMINANCE16 ] = "GL_LUMINANCE16";
    mInternalFormatMap[ GL_LUMINANCE4_ALPHA4 ] = "GL_LUMINANCE4_ALPHA4";
    mInternalFormatMap[ GL_LUMINANCE6_ALPHA2 ] = "GL_LUMINANCE6_ALPHA2";
    mInternalFormatMap[ GL_LUMINANCE8_ALPHA8 ] = "GL_LUMINANCE8_ALPHA8";
    mInternalFormatMap[ GL_LUMINANCE12_ALPHA4 ] = "GL_LUMINANCE12_ALPHA4";
    mInternalFormatMap[ GL_LUMINANCE12_ALPHA12 ] = "GL_LUMINANCE12_ALPHA12";
    mInternalFormatMap[ GL_LUMINANCE16_ALPHA16 ] = "GL_LUMINANCE16_ALPHA16";
    mInternalFormatMap[ GL_INTENSITY ] = "GL_INTENSITY";
    mInternalFormatMap[ GL_INTENSITY4 ] = "GL_INTENSITY4";
    mInternalFormatMap[ GL_INTENSITY8 ] = "GL_INTENSITY8";
    mInternalFormatMap[ GL_INTENSITY12 ] = "GL_INTENSITY12";
    mInternalFormatMap[ GL_INTENSITY16 ] = "GL_INTENSITY16";
    mInternalFormatMap[ GL_R3_G3_B2 ] = "GL_R3_G3_B2";
    mInternalFormatMap[ GL_RGB4 ] = "GL_RGB4";
    mInternalFormatMap[ GL_RGB5 ] = "GL_RGB5";
    mInternalFormatMap[ GL_RGB8 ] = "GL_RGB8";
    mInternalFormatMap[ GL_RGB10 ] = "GL_RGB10";
    mInternalFormatMap[ GL_RGB12 ] = "GL_RGB12";
    mInternalFormatMap[ GL_RGB16 ] = "GL_RGB16";
    mInternalFormatMap[ GL_RGBA2 ] = "GL_RGBA2";
    mInternalFormatMap[ GL_RGBA4 ] = "GL_RGBA4";
    mInternalFormatMap[ GL_RGB5_A1 ] = "GL_RGB5_A1";
    mInternalFormatMap[ GL_RGBA8 ] = "GL_RGBA8";
    mInternalFormatMap[ GL_RGB10_A2 ] = "GL_RGB10_A2";
    mInternalFormatMap[ GL_RGBA12 ] = "GL_RGBA12";
    mInternalFormatMap[ GL_RGBA16 ] = "GL_RGBA16";
}

// Initializes the shader and vertex buffers required for rendering the primary render target to screen,
// using a variety of post-processing effects.
void RenderSystem::InitializePostProcessing()
{
    m_pPostProcessShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>( "data/shaders/postprocessing.glsl" );
    m_pPostProcessShader->RegisterForgeRebuildCallback( this,
        [ this ]() {
            RegisterPostProcessingUniforms();
        } );

    RegisterPostProcessingUniforms();

    const float w = static_cast<float>( m_pPrimaryViewport->GetWidth() );
    const float h = static_cast<float>( m_pPrimaryViewport->GetHeight() );
    m_pPostProcessVertexBuffer = new VertexBuffer( GeometryType::Triangle, VBO_POSITION | VBO_UV );
    m_pPostProcessVertexBuffer->CreateUntexturedQuad( 0.0f, 0.0f, w, h );

    const float uvs[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f };
    m_pPostProcessVertexBuffer->CopyData( uvs, 12, VBO_UV );
}

void RenderSystem::RegisterPostProcessingUniforms()
{
    ShaderUniformSharedPtr pBaseSampler = m_pPostProcessShader->RegisterUniform( "k_sampler0", ShaderUniformType::Texture );
    pBaseSampler->Set( m_pPrimaryViewport->GetRenderTarget()->GetColor(), GL_TEXTURE0 );

    // RenderTargetId::GlowBlurVertical contains the final result of the glow effect.
    ShaderUniformSharedPtr pGlowSampler = m_pPostProcessShader->RegisterUniform( "k_sampler1", ShaderUniformType::Texture );
    pGlowSampler->Set( m_GlowVerticalBlurRenderTarget->GetColor(), GL_TEXTURE1 );

    ShaderUniformSharedPtr pResolution = m_pPostProcessShader->RegisterUniform( "k_resolution", ShaderUniformType::FloatVector2 );
    const float w = static_cast<float>( m_pPrimaryViewport->GetWidth() );
    const float h = static_cast<float>( m_pPrimaryViewport->GetHeight() );
    if ( pResolution != nullptr )
    {
        pResolution->Set( glm::vec2( w, h ) );
    }

    auto LinkPostProcessEffect = [ this ]( const char* pUniformName, PostProcessEffect effect ) {
        ShaderUniformSharedPtr pShaderUniform = m_pPostProcessShader->RegisterUniform( pUniformName, ShaderUniformType::Boolean );
        if ( pShaderUniform != nullptr )
        {
            pShaderUniform->Set( Configuration::IsPostProcessingEffectEnabled( effect ) );
            m_PostProcessShaderUniforms[ static_cast<size_t>( effect ) ] = pShaderUniform;
        }
    };

    LinkPostProcessEffect( "k_applyBleachBypass", PostProcessEffect::BleachBypass );
    LinkPostProcessEffect( "k_applyGlow", PostProcessEffect::Glow );
    LinkPostProcessEffect( "k_applyVignette", PostProcessEffect::Vignette );
}

// Initializes the shader and vertex buffer used by the glow effect.
void RenderSystem::InitializeGlowChain()
{
    m_pGlowShader = FrameWork::GetResourceManager()->GetResource<ResourceShader*>( "data/shaders/glow.glsl" );

    // The sampler isn't set immediately as it alternates between the two glow blur render targets.
    m_pGlowShaderSampler = m_pGlowShader->RegisterUniform( "k_sampler0", ShaderUniformType::Texture );

    // Horizontal (1,0) or vertical (0,1) blur.
    m_pGlowShaderDirection = m_pGlowShader->RegisterUniform( "k_direction", ShaderUniformType::FloatVector2 );

    ShaderUniformSharedPtr pResolution = m_pGlowShader->RegisterUniform( "k_resolution", ShaderUniformType::FloatVector2 );
    const float w = static_cast<float>( m_GlowRenderTarget->GetWidth() );
    const float h = static_cast<float>( m_GlowRenderTarget->GetHeight() );
    if ( pResolution != nullptr )
    {
        pResolution->Set( glm::vec2( w, h ) );
    }

    m_pGlowVertexBuffer = new VertexBuffer( GeometryType::Triangle, VBO_POSITION | VBO_UV );
    m_pGlowVertexBuffer->CreateTexturedQuad( 0.0f, 0.0f, w, h );
}

std::string RenderSystem::ConvertInternalFormatToString( GLenum format )
{
    InternalFormatMap::iterator it = mInternalFormatMap.find( format );
    if ( it != mInternalFormatMap.end() )
        return it->second;
    return "Unknown";
}

std::string RenderSystem::GetRenderbufferParameters( GLuint id )
{
    if ( glIsRenderbuffer( id ) == GL_FALSE )
        return "Not Renderbuffer object";

    int width, height, format;
    std::string formatName;
    glBindRenderbuffer( GL_RENDERBUFFER, id );
    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width ); // get renderbuffer width
    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height ); // get renderbuffer height
    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format ); // get renderbuffer internal format
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    formatName = ConvertInternalFormatToString( format );

    std::stringstream ss;
    ss << width << "x" << height << ", " << formatName;
    return ss.str();
}

std::string RenderSystem::GetTextureParameters( GLuint id )
{
    if ( glIsTexture( id ) == GL_FALSE )
        return "Not texture object";

    int width, height, format;
    glBindTexture( GL_TEXTURE_2D, id );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width ); // get texture width
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height ); // get texture height
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format ); // get texture internal format
    glBindTexture( GL_TEXTURE_2D, 0 );

    std::stringstream ss;
    ss << width << "x" << height << ", " << ConvertInternalFormatToString( format );
    return ss.str();
}

void RenderSystem::PrintFramebufferInfo( GLuint fboId )
{
    glBindFramebuffer( GL_FRAMEBUFFER, fboId );

    // print max # of color buffers supported by FBO
    int colorBufferCount = 0;
    glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &colorBufferCount );
    Log::Info() << "> Max Number of Color Buffer Attachment Points: " << colorBufferCount;

    int objectType;
    int objectId;

    // print info of the color buffer attachable image
    for ( int i = 0; i < colorBufferCount; ++i )
    {
        glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType );
        if ( objectType != GL_NONE )
        {
            glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId );

            std::stringstream output;
            output << "> Color Attachment " << i << ": ";
            if ( objectType == GL_TEXTURE )
                output << "GL_TEXTURE, " << GetTextureParameters( objectId );
            else if ( objectType == GL_RENDERBUFFER )
                output << "GL_RENDERBUFFER, " << GetRenderbufferParameters( objectId );
            Log::Info() << output.str();
        }
    }

    // print info of the depth buffer attachable image
    glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType );
    if ( objectType != GL_NONE )
    {
        glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId );

        std::stringstream output;
        output << "> Depth Attachment: ";
        switch ( objectType )
        {
        case GL_TEXTURE:
            output << "GL_TEXTURE, " << GetTextureParameters( objectId );
            break;
        case GL_RENDERBUFFER:
            output << "GL_RENDERBUFFER, " << GetRenderbufferParameters( objectId );
            break;
        }
        Log::Info() << output.str();
    }

    // print info of the stencil buffer attachable image
    glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType );
    if ( objectType != GL_NONE )
    {
        glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objectId );

        std::stringstream output;
        output << "> Stencil Attachment: ";
        switch ( objectType )
        {
        case GL_TEXTURE:
            output << "GL_TEXTURE, " << GetTextureParameters( objectId );
            break;
        case GL_RENDERBUFFER:
            output << "GL_RENDERBUFFER, " << GetRenderbufferParameters( objectId );
            break;
        }
        Log::Info() << output.str();
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderSystem::SetRenderTarget( const RenderTargetSharedPtr& pRenderTarget )
{
    SDL_assert( pRenderTarget != nullptr );
    glBindFramebuffer( GL_FRAMEBUFFER, pRenderTarget->GetFBO() );
    glViewport( 0, 0, pRenderTarget->GetWidth(), pRenderTarget->GetHeight() );
}

void RenderSystem::SetDefaultRenderTarget()
{
    SetRenderTarget( m_pPrimaryViewport->GetRenderTarget() );
}

void RenderSystem::SetGlowRenderTarget()
{
    SetRenderTarget( m_GlowRenderTarget );
}

void RenderSystem::RenderGlow()
{
    RenderTarget* pGlowRenderTarget = m_GlowRenderTarget.get();
    m_ViewMatrix = glm::mat4();
    m_ProjectionMatrix = glm::ortho( 0.0f, static_cast<float>( pGlowRenderTarget->GetWidth() ), static_cast<float>( pGlowRenderTarget->GetHeight() ), 0.0f, -1.0f, 1.0f );

    SetRenderTarget( m_GlowHorizontalBlurRenderTarget );
    m_pGlowShaderSampler->Set( pGlowRenderTarget->GetColor(), GL_TEXTURE0 );
    m_pGlowShaderDirection->Set( glm::vec2( 1.0f, 0.0f ) );
    m_pGlowShader->Use();
    m_pGlowVertexBuffer->Draw();

    SetRenderTarget( m_GlowVerticalBlurRenderTarget );
    m_pGlowShaderSampler->Set( m_GlowHorizontalBlurRenderTarget->GetColor(), GL_TEXTURE0 );
    m_pGlowShaderDirection->Set( glm::vec2( 0.0f, 1.0f ) );
    m_pGlowShader->Use();
    m_pGlowVertexBuffer->Draw();
}

TaskStatus RenderSystem::Update( float delta )
{
    m_ShaderTimer += delta;

    ClearAll();

    for ( auto& pViewport : m_Viewports )
    {
        ViewPerspective( pViewport->GetWidth(), pViewport->GetHeight(), pViewport->GetScene(), pViewport->GetCamera() );
        m_pCurrentViewport = pViewport;
        pViewport->Render();
    }

    m_pCurrentViewport = m_pPrimaryViewport;
    RenderGlow();

    // Render directly to screen now.
    glViewport( 0, 0, Configuration::GetScreenWidth(), Configuration::GetScreenHeight() );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    ViewOrtho();
    m_pPostProcessShader->Use();
    m_pPostProcessVertexBuffer->Draw();

    SetBlendMode( BlendMode::Disabled );
    FrameWork::GetGuiManager()->Render( delta );
    DrawDebugWindow();
    ImGui::Render();
    ImGuiImpl::Render();
    m_pCurrentViewport = nullptr;

    FrameWork::GetWindow()->Present();

    if ( IsScreenshotScheduled() )
    {
        TakeScreenshotAux( true );
    }

    return TaskStatus::Continue;
}

void RenderSystem::ViewOrtho( int width /* = 0 */, int height /* = 0 */ )
{
    if ( width == 0 )
    {
        width = m_ScreenWidth;
    }

    if ( height == 0 )
    {
        height = m_ScreenHeight;
    }

    m_ViewMatrix = glm::mat4();
    m_ProjectionMatrix = glm::ortho( 0.0f, static_cast<float>( width ), static_cast<float>( height ), 0.0f, -1.0f, 1.0f );
}

void RenderSystem::ViewPerspective( int width /* = 0 */, int height /* = 0 */, SceneSharedPtr pScene /* = nullptr */, SceneCameraSharedPtr pCamera /* = nullptr */ )
{
    width = ( width == 0 ) ? m_ScreenWidth : width;
    height = ( height == 0 ) ? m_ScreenHeight : height;
    pScene = ( pScene == nullptr ) ? FrameWork::GetScene() : pScene;

    if ( pCamera == nullptr )
    {
        pCamera = pScene->GetCamera();
    }

    glm::vec3 cPos = pCamera->GetPosition();
    glm::vec3 cTgt = pCamera->GetTargetPosition();

    // OpenGL's up is +Y.
    m_ViewMatrix = glm::lookAt( glm::vec3( cPos.x, cPos.y, cPos.z ), glm::vec3( cTgt.x, cTgt.y, cTgt.z ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

    m_ProjectionMatrix = glm::perspective( 45.0f, static_cast<float>( width ) / static_cast<float>( height ), 1.0f, 1000.0f );
}

IntersectionResult RenderSystem::LinePlaneIntersection( const glm::vec3& position, const glm::vec3& direction, const glm::vec3& planePosition, const glm::vec3& planeNormal, glm::vec3& result )
{
    const glm::vec3 u = direction;
    const glm::vec3 w = position - planePosition;

    const float D = glm::dot( planeNormal, direction );
    const float N = -glm::dot( planeNormal, w );

    if ( fabs( D ) < 0.00001f ) // segment is parallel to plane
    {
        if ( N == 0 ) // segment lies in plane
        {
            return IntersectionResult::FailureCoplanar;
        }
        else // no intersection
        {
            return IntersectionResult::FailureParallel;
        }
    }

    // they are not parallel
    // compute intersect param
    const float sI = N / D;
    result = position + sI * u; // compute segment intersect point
    return IntersectionResult::Success;
}

void RenderSystem::ScreenPosToWorldRay( int mouseX, int mouseY, // Mouse position, in pixels, from bottom-left corner of the window
    int screenWidth, int screenHeight, // Window size, in pixels
    const glm::mat4& ViewMatrix, // Camera position and orientation
    const glm::mat4& ProjectionMatrix, // Camera parameters (ratio, field of view, near and far planes)
    glm::vec3& out_origin, // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
    glm::vec3& out_direction // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
)
{
    // The ray Start and End positions, in Normalized Device Coordinates
    glm::vec4 lRayStart_NDC( ( (float)mouseX / (float)screenWidth - 0.5f ) * 2.0f, // [0,1024] -> [-1,1]
        ( (float)mouseY / (float)screenHeight - 0.5f ) * 2.0f, // [0, 768] -> [-1,1]
        -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
        1.0f );
    glm::vec4 lRayEnd_NDC( ( (float)mouseX / (float)screenWidth - 0.5f ) * 2.0f, ( (float)mouseY / (float)screenHeight - 0.5f ) * 2.0f, 0.0, 1.0f );

    glm::mat4 M = glm::inverse( ProjectionMatrix * ViewMatrix );
    glm::vec4 lRayStart_world = M * lRayStart_NDC;
    lRayStart_world /= lRayStart_world.w;
    glm::vec4 lRayEnd_world = M * lRayEnd_NDC;
    lRayEnd_world /= lRayEnd_world.w;

    glm::vec3 lRayDir_world( lRayEnd_world - lRayStart_world );
    lRayDir_world = glm::normalize( lRayDir_world );

    out_origin = glm::vec3( lRayStart_world );
    out_direction = glm::normalize( lRayDir_world );
}

glm::vec3 RenderSystem::Raycast( const glm::vec2& screenCoordinates )
{
    glm::vec3 origin, direction;
    ScreenPosToWorldRay( (int)screenCoordinates.x, m_ScreenHeight - (int)screenCoordinates.y, m_ScreenWidth, m_ScreenHeight, m_ViewMatrix, m_ProjectionMatrix, origin, direction );

    // Intersection with the Z plane
    const glm::vec3 planePosition( 0.0f, 0.0f, 0.0f );
    const glm::vec3 planeNormal( 0.0f, 0.0f, 1.0f );

    glm::vec3 result;
    IntersectionResult intersectionResult = LinePlaneIntersection( origin, direction, planePosition, planeNormal, result );
    SDL_assert( intersectionResult == IntersectionResult::Success );

    return result;
}

void RenderSystem::TakeScreenshot()
{
    InputManager* pInputManager = FrameWork::GetInputManager();
    const bool immediateScreenshot = !( pInputManager->IsButtonPressed( SDL_SCANCODE_LCTRL ) || pInputManager->IsButtonPressed( SDL_SCANCODE_RCTRL ) );
    TakeScreenshotAux( immediateScreenshot );
}

void RenderSystem::TakeScreenshotAux( bool immediate )
{
    if ( immediate )
    {
        std::string filename;
        if ( GetScreenshotFilename( filename ) )
        {
            SDL_Surface* pSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, m_ScreenWidth, m_ScreenHeight, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0 );
            glReadPixels( 0, 0, m_ScreenWidth, m_ScreenHeight, GL_RGB, GL_UNSIGNED_BYTE, pSurface->pixels );

            // The image needs to be flipped vertically before being dumped into the BMP
            SDL_Surface* pFlippedSurface = FlipSurfaceVertical( pSurface );
            SDL_SaveBMP( pFlippedSurface, filename.c_str() );
            SDL_FreeSurface( pSurface );
            SDL_FreeSurface( pFlippedSurface );

            Log::Info() << "Screenshot taken: " << filename;
        }
        else
        {
            Log::Warning() << "Couldn't take screenshot.";
        }

        m_ScreenshotScheduled = false;
    }
    else
    {
        Log::Info() << "Screenshot scheduled...";
        m_ScreenshotScheduled = true;
    }
}

void RenderSystem::Capture()
{
    if ( IsCaptureInProgress() )
    {
        EndCapture();
    }
    else
    {
        BeginCapture();
    }
}

// Function courtesy of Sebastian Beschke s.beschke@gmx.de, adapted to C
// From http://lists.libsdl.org/pipermail/sdl-libsdl.org/2005-January/047963.html
SDL_Surface* RenderSystem::FlipSurfaceVertical( SDL_Surface* pSurface ) const
{
    SDL_Surface* pFlippedSurface = SDL_CreateRGBSurface( pSurface->flags, pSurface->w, pSurface->h, pSurface->format->BytesPerPixel * 8, pSurface->format->Rmask, pSurface->format->Gmask,
        pSurface->format->Bmask, pSurface->format->Amask );
    Uint8* pixels = (Uint8*)pSurface->pixels;
    Uint8* rpixels = (Uint8*)pFlippedSurface->pixels;
    Uint32 pitch = pSurface->pitch;
    Uint32 pxlength = pitch * pSurface->h;

    for ( Uint32 line = 0; line < (Uint32)pSurface->h; ++line )
    {
        Uint32 pos = line * pitch;
        memcpy( &rpixels[ pxlength - pos - pitch ], &pixels[ pos ], pitch );
    }

    return pFlippedSurface;
}

bool RenderSystem::GetScreenshotFilename( std::string& filename ) const
{
    for ( int counter = 1; counter <= 999; ++counter )
    {
        std::stringstream ss;
        ss << "Screenshot" << counter << ".bmp";
        std::string temporaryFilename = ss.str();

        std::ifstream file( temporaryFilename.c_str() );
        if ( file.good() )
        {
            file.close();
        }
        else
        {
            file.close();
            filename = temporaryFilename;
            return true;
        }
    }

    return false;
}

void RenderSystem::SetBlendMode( BlendMode blendMode )
{
    if ( GetBlendMode() == blendMode )
    {
        return;
    }
    else if ( blendMode == BlendMode::Disabled )
    {
        glDisable( GL_BLEND );
        m_BlendMode = BlendMode::Disabled;
        return;
    }
    else if ( GetBlendMode() == BlendMode::Disabled )
    {
        glEnable( GL_BLEND );
    }

    if ( blendMode == BlendMode::Blend )
    {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        m_BlendMode = BlendMode::Blend;
    }
    else if ( blendMode == BlendMode::Add )
    {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );
        m_BlendMode = BlendMode::Add;
    }
    else if ( blendMode == BlendMode::Screen )
    {
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );
        m_BlendMode = BlendMode::Screen;
    }
    else if ( blendMode == BlendMode::Multiply )
    {
        glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
        m_BlendMode = BlendMode::Multiply;
    }
}

void RenderSystem::EnablePostProcessEffect( PostProcessEffect effect, bool enable )
{
    const size_t idx = static_cast<size_t>( effect );
    m_ActivePostProcessEffects[ idx ] = enable;
    m_PostProcessShaderUniforms[ idx ]->Set( enable );
}

void RenderSystem::AddViewport( const ViewportSharedPtr& pViewport )
{
    m_Viewports.push_back( pViewport );
}

void RenderSystem::RemoveViewport( const ViewportSharedPtr& pViewport )
{
    m_Viewports.remove( pViewport );
}

} // namespace Genesis