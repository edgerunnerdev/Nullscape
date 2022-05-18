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

#pragma once

#include <list>

#define VPX_CODEC_DISABLE_COMPAT 1
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

#include "rendersystem.fwd.h"
#include "inputmanager.h"
#include "taskmanager.h"

namespace Genesis
{

static const unsigned int IVF_FILE_HDR_SZ = 32;
static const unsigned int IVF_FRAME_HDR_SZ = 12;

class ResourceVideo;
class VertexBuffer;
class Shader;
class ShaderUniform;

class VideoPlayer : public Task
{
public:
    VideoPlayer();
    virtual ~VideoPlayer() override;

    TaskStatus Update( float delta );
    void Render( GLuint& outputTexture );

    void Play( ResourceVideo* pResourceVideo );
    bool IsPlaying() const;
    void Skip();

private:
    void PlayVPXInit( const std::string& filename );
    bool PlayVPXLoop();
    int PlayVPXGetTexture();
    void PlayVPXDeinit();
    void PlayVPXDieCodec( const char* s );
    void PlayVPXConvertToRGB();

    struct Vpxdata
    {
        bool is_init;
        std::string filename;
        int state;
        unsigned char* pixels;
        GLuint texture;

        FILE* infile;
        vpx_codec_ctx_t codec;
        int flags, frame_cnt;
        unsigned char file_hdr[ IVF_FILE_HDR_SZ ];
        unsigned char frame_hdr[ IVF_FRAME_HDR_SZ ];
        unsigned char frame[ 256 * 1024 ];
        vpx_codec_err_t res;

        int frame_sz;
        vpx_codec_iter_t iter;
        vpx_image_t* img;
    };

    typedef std::list<ResourceVideo*> ResourceVideoList;
    ResourceVideoList m_Queue;
    bool m_Skip;
    Vpxdata m_Data;
    Uint32 m_NextFrame;
    InputCallbackToken m_SkipKeyPressedToken;
};
}