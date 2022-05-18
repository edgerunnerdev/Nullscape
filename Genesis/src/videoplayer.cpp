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

#include <sstream>

#include "configuration.h"
#include "genesis.h"
#include "logger.h"
#include "rendersystem.h"
#include "resources/resourcevideo.h"
#include "shader.h"
#include "videoplayer.h"

#include "resourcemanager.h"
#include "resources/resourceimage.h"

#define vpxInterface ( vpx_codec_vp8_dx() )

namespace Genesis
{

static unsigned int mem_get_le32( const uint8_t* mem )
{
    return ( mem[ 3 ] << 24 ) | ( mem[ 2 ] << 16 ) | ( mem[ 1 ] << 8 ) | ( mem[ 0 ] );
}

VideoPlayer::VideoPlayer()
    : m_Skip( false )
    , m_NextFrame( 0 )
{
    memset( &m_Data, 0, sizeof( m_Data ) );

    m_SkipKeyPressedToken = FrameWork::GetInputManager()->AddKeyboardCallback( std::bind( &VideoPlayer::Skip, this ), SDL_SCANCODE_ESCAPE, ButtonState::Pressed );
}

VideoPlayer::~VideoPlayer()
{
    if ( m_Data.is_init )
    {
        PlayVPXDeinit();
    }

    if ( FrameWork::GetInputManager() != nullptr )
    {
        FrameWork::GetInputManager()->RemoveKeyboardCallback( m_SkipKeyPressedToken );
    }
}

TaskStatus VideoPlayer::Update( float delta )
{
    // Do we have something to play?
    bool framePlayed = false;
    while ( m_Queue.empty() == false && framePlayed == false )
    {
        // Are we initialised?
        if ( m_Data.is_init == false )
        {
            ResourceVideo* pResourceVideo = m_Queue.front();
            PlayVPXInit( pResourceVideo->GetFilename().GetFullPath().c_str() );
        }

        // Our videos are played at 30 FPS
        uint32_t ticks = SDL_GetTicks();
        if ( ticks >= m_NextFrame )
        {
            framePlayed = PlayVPXLoop();
            m_NextFrame = m_NextFrame + 33;
        }
        else
        {
            framePlayed = true;
        }

        if ( m_Skip == true || framePlayed == false )
        {
            PlayVPXDeinit();
            m_Queue.pop_front();
        }
    }

    m_Skip = false;

    return TaskStatus::Continue;
}

void VideoPlayer::Render( GLuint& outputTexture )
{
    if ( m_Data.is_init )
    {
        outputTexture = PlayVPXGetTexture();
    }
    else
    {
        outputTexture = 0;
    }
}

void VideoPlayer::Play( ResourceVideo* pResourceVideo )
{
    if ( pResourceVideo != nullptr )
    {
        // Set the next frame to be played right now.
        if ( m_Queue.empty() )
        {
            m_NextFrame = SDL_GetTicks();
        }

        m_Queue.push_back( pResourceVideo );
    }
}

bool VideoPlayer::IsPlaying() const
{
    return ( m_Queue.empty() == false );
}

void VideoPlayer::Skip()
{
    if ( m_Queue.empty() == false && m_Queue.front()->IsSkippable() )
    {
        m_Skip = true;
    }
}

void VideoPlayer::PlayVPXInit( const std::string& filename )
{
    memset( &m_Data, 0, sizeof( m_Data ) );
    m_Data.filename = filename;

#ifdef _WIN32
    errno_t err = fopen_s( &m_Data.infile, m_Data.filename.c_str(), "rb" );
    if ( err != 0 )
    {
        FrameWork::GetLogger()->LogError( "Unable to open file: '%s'", m_Data.filename.c_str() );
        return;
    }
#else
    m_Data.infile = fopen( m_Data.filename.c_str(), "rb" );
    if ( m_Data.infile == nullptr )
    {
        FrameWork::GetLogger()->LogError( "Unable to open file: '%s'", m_Data.filename.c_str() );
        return;
    }
#endif

    // Read file header
    if ( !( fread( m_Data.file_hdr, 1, IVF_FILE_HDR_SZ, m_Data.infile ) == IVF_FILE_HDR_SZ
             && m_Data.file_hdr[ 0 ] == 'D' && m_Data.file_hdr[ 1 ] == 'K' && m_Data.file_hdr[ 2 ] == 'I'
             && m_Data.file_hdr[ 3 ] == 'F' ) )
    {
        FrameWork::GetLogger()->LogError( "Not an IVF file: '%s'", m_Data.filename.c_str() );
        return;
    }

    // Initialize codec
    FrameWork::GetLogger()->LogInfo( "Using %s", vpx_codec_iface_name( vpxInterface ) );
    if ( vpx_codec_dec_init( &m_Data.codec, vpxInterface, nullptr, m_Data.flags ) )
    {
        PlayVPXDieCodec( "Failed to initialize decoder" );
        return;
    }

    m_Data.is_init = true;
}

bool VideoPlayer::PlayVPXLoop()
{
    if ( !m_Data.is_init )
    {
        return false;
    }

    if ( m_Data.state == -1 )
    {
        return false;
    }

    if ( !( fread( m_Data.frame_hdr, 1, IVF_FRAME_HDR_SZ, m_Data.infile ) == IVF_FRAME_HDR_SZ ) )
    {
        return false;
    }

    m_Data.frame_sz = mem_get_le32( m_Data.frame_hdr );
    m_Data.iter = NULL;
    m_Data.frame_cnt++;

    if ( m_Data.frame_sz > (int)sizeof( m_Data.frame ) )
    {
        FrameWork::GetLogger()->LogError( "Frame %d data too big for example code buffer", m_Data.frame_sz );
        m_Data.state = -1;
        return false;
    }

    if ( (int)fread( m_Data.frame, 1, m_Data.frame_sz, m_Data.infile ) != m_Data.frame_sz )
    {
        FrameWork::GetLogger()->LogError( "Frame %d failed to read complete frame", m_Data.frame_cnt );
        m_Data.state = -1;
        return false;
    }

    // Decode the frame
    if ( vpx_codec_decode( &m_Data.codec, m_Data.frame, m_Data.frame_sz, nullptr, 0 ) )
    {
        FrameWork::GetLogger()->LogError( "Failed to decode frame" );
        m_Data.state = -1;
        return false;
    }

    // NOTE: if multiple frames come in somehow (not supposed to happen?) this won't handle it properly
    int n = 0;
    vpx_image_t* img;
    while ( ( img = vpx_codec_get_frame( &m_Data.codec, &m_Data.iter ) ) != 0 )
    {
        m_Data.img = img;
        n += 1;
    }
    if ( n > 1 )
    {
        fprintf( stderr, "%d frames in 1\n", n );
    }

    return true;
}

void VideoPlayer::PlayVPXConvertToRGB()
{
    if ( !m_Data.is_init )
    {
        return;
    }
    if ( m_Data.state == -1 )
    {
        return;
    }
    if ( !m_Data.img )
    {
        return;
    }

    vpx_image_t* img = m_Data.img;
    const int w = img->d_w;
    const int w2 = w / 2;
    const int pstride = w * 3;
    const int h = img->d_h;
    const int h2 = h / 2;
    if ( !m_Data.pixels )
    {
        m_Data.pixels = (unsigned char*)malloc( w * h * 3 );
    }

    const int strideY = img->stride[ 0 ];
    const int strideU = img->stride[ 1 ];
    const int strideV = img->stride[ 2 ];

    for ( int posy = 0; posy < h2; posy++ )
    {
        uint8_t* dst = m_Data.pixels + pstride * ( posy * 2 );
        uint8_t* dst2 = m_Data.pixels + pstride * ( posy * 2 + 1 );
        const uint8_t* srcY = img->planes[ 0 ] + strideY * posy * 2;
        const uint8_t* srcY2 = img->planes[ 0 ] + strideY * ( posy * 2 + 1 );
        const uint8_t* srcU = img->planes[ 1 ] + strideU * posy;
        const uint8_t* srcV = img->planes[ 2 ] + strideV * posy;

        for ( int posx = 0; posx < w2; posx++ )
        {
            uint8_t Y, U, V;
            int16_t R, G, B;
            int16_t iR, iG, iB;

            U = *( srcU++ );
            V = *( srcV++ );
            iR = ( 351 * ( V - 128 ) ) / 256;
            iG = -( 179 * ( V - 128 ) ) / 256 - ( 86 * ( U - 128 ) ) / 256;
            iB = ( 444 * ( U - 128 ) ) / 256;

            Y = *( srcY++ );
            R = Y + iR;
            G = Y + iG;
            B = Y + iB;
            R = ( R < 0 ? 0 : ( R > 255 ? 255 : R ) );
            G = ( G < 0 ? 0 : ( G > 255 ? 255 : G ) );
            B = ( B < 0 ? 0 : ( B > 255 ? 255 : B ) );
            *( dst++ ) = (uint8_t)R;
            *( dst++ ) = (uint8_t)G;
            *( dst++ ) = (uint8_t)B;

            Y = *( srcY2++ );
            R = Y + iR;
            G = Y + iG;
            B = Y + iB;
            R = ( R < 0 ? 0 : ( R > 255 ? 255 : R ) );
            G = ( G < 0 ? 0 : ( G > 255 ? 255 : G ) );
            B = ( B < 0 ? 0 : ( B > 255 ? 255 : B ) );
            *( dst2++ ) = (uint8_t)R;
            *( dst2++ ) = (uint8_t)G;
            *( dst2++ ) = (uint8_t)B;

            Y = *( srcY++ );
            R = Y + iR;
            G = Y + iG;
            B = Y + iB;
            R = ( R < 0 ? 0 : ( R > 255 ? 255 : R ) );
            G = ( G < 0 ? 0 : ( G > 255 ? 255 : G ) );
            B = ( B < 0 ? 0 : ( B > 255 ? 255 : B ) );
            *( dst++ ) = (uint8_t)R;
            *( dst++ ) = (uint8_t)G;
            *( dst++ ) = (uint8_t)B;

            Y = *( srcY2++ );
            R = Y + iR;
            G = Y + iG;
            B = Y + iB;
            R = ( R < 0 ? 0 : ( R > 255 ? 255 : R ) );
            G = ( G < 0 ? 0 : ( G > 255 ? 255 : G ) );
            B = ( B < 0 ? 0 : ( B > 255 ? 255 : B ) );
            *( dst2++ ) = (uint8_t)R;
            *( dst2++ ) = (uint8_t)G;
            *( dst2++ ) = (uint8_t)B;
        }
    }
}

int VideoPlayer::PlayVPXGetTexture()
{
    if ( !m_Data.is_init )
    {
        return 0;
    }
    if ( m_Data.state == -1 )
    {
        return 0;
    }
    if ( !m_Data.img )
    {
        return 0;
    }

    PlayVPXConvertToRGB();

    if ( !m_Data.texture )
    {
        glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
        glGenTextures( 1, &m_Data.texture );
        glBindTexture( GL_TEXTURE_2D, m_Data.texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    }

    glBindTexture( GL_TEXTURE_2D, m_Data.texture );
    int width = m_Data.img->d_w;
    int height = m_Data.img->d_h;
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Data.pixels );

    return m_Data.texture;
}

void VideoPlayer::PlayVPXDeinit()
{
    if ( !m_Data.is_init )
    {
        return;
    }

    FrameWork::GetLogger()->LogInfo( "Processed %d frames.", m_Data.frame_cnt );
    if ( vpx_codec_destroy( &m_Data.codec ) )
    {
        FrameWork::GetLogger()->LogError( "Failed to destroy codec" );
    }

    if ( m_Data.infile )
    {
        fclose( m_Data.infile );
    }

    if ( m_Data.pixels )
    {
        free( m_Data.pixels );
    }

    if ( m_Data.texture )
    {
        glDeleteTextures( 1, &m_Data.texture );
    }

    memset( &m_Data, 0, sizeof( m_Data ) );
}

void VideoPlayer::PlayVPXDieCodec( const char* s )
{
    std::stringstream ss;
    ss << s << ": " << vpx_codec_error( &m_Data.codec );

    const char* pDetails = vpx_codec_error_detail( &m_Data.codec );
    if ( pDetails != nullptr )
    {
        ss << "\n"
           << "Details: " << pDetails;
    }

    FrameWork::GetLogger()->LogWarning( ss.str().c_str() );
}
}