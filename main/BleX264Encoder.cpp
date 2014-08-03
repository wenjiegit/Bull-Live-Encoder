/*
The MIT License (MIT)

Copyright (c) wenjie.zhao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "BleX264Encoder.hpp"

#include "MOption.h"
#include "BleUtil.hpp"
#include "BleErrno.hpp"

#include <QSize>
#include <QDebug>

BleX264Encoder::BleX264Encoder()
    : m_x264Param(NULL)
    , m_pictureIn(NULL)
    , m_x264Encoder(NULL)
{
}

BleX264Encoder::~BleX264Encoder()
{
    fini();
}

int BleX264Encoder::init()
{
    MOption *option = MOption::instance();

    QString presetName  = option->option("preset", "x264").toString();
    //QString tuneName    = option->option("tune", "x264").toString();
    QString profileName = option->option("profile", "x264").toString();
    int fps = option->option("fps", "encoder").toInt();
    int bps = option->option("bitrate", "encoder").toInt();

    QSize wh = option->option("res", "encoder").toSize();
    int width = wh.width();
    int height = wh.height();

    int maxBitRate = bps;
    int bufferSize = maxBitRate;
    bool bUseCBR = (option->option("BitrateMode", "x264").toString() == "CBR");
    static const float baseCRF = 20.0f;
    int quality = option->option("quality", "x264").toInt();
    int KeyFrameInterval = option->option("KeyFrameInterval", "x264").toInt();

    m_x264Param = new x264_param_t;

    x264_param_default_preset(m_x264Param , presetName.toStdString().c_str(), NULL);
    x264_param_apply_profile(m_x264Param, profileName.toStdString().c_str());

    if(bUseCBR)
    {
        m_x264Param->rc.i_bitrate          = maxBitRate;
        m_x264Param->rc.i_vbv_max_bitrate  = maxBitRate; //vbv-maxrate
        m_x264Param->rc.i_vbv_buffer_size  = bufferSize; //vbv-bufsize
        m_x264Param->i_nal_hrd             = X264_NAL_HRD_CBR;
        m_x264Param->rc.i_rc_method        = X264_RC_ABR;
        m_x264Param->rc.f_rf_constant      = 0.0f;
    }
    else
    {
        m_x264Param->rc.i_vbv_max_bitrate  = maxBitRate; //vbv-maxrate
        m_x264Param->rc.i_vbv_buffer_size  = bufferSize; //vbv-bufsize
        m_x264Param->rc.i_rc_method        = X264_RC_CRF;
        m_x264Param->rc.f_rf_constant      = baseCRF+float(10-quality);
    }

    m_x264Param->b_vfr_input           = 1;
    m_x264Param->i_keyint_max          = fps*KeyFrameInterval;      //keyframe every 5 sec, should make this an option
    m_x264Param->i_width               = width;
    m_x264Param->i_height              = height;
    m_x264Param->vui.b_fullrange       = 0;          //specify full range input levels
    //m_x264Param->i_frame_reference
    //m_x264Param->b_in

    m_x264Param->i_fps_num = fps;
    m_x264Param->i_fps_den = 1;

    m_x264Param->i_timebase_num = 1;
    m_x264Param->i_timebase_den = 1000;

    // 00 00 00 01 before NAL
    m_x264Param->b_repeat_headers = 1;
    m_x264Param->b_annexb = 1;

    // enabled one thread
//    m_x264Param->i_threads = 1;
//    m_x264Param->i_slice_count = 1;
//    m_x264Param->i_slice_max_mbs = 0;
//    m_x264Param->i_slice_max_size = 0;
    m_x264Param->i_bframe = 0;


    // cpu capabilities
    m_x264Param->cpu = 0;
    m_x264Param->cpu |=X264_CPU_MMX;
    m_x264Param->cpu |=X264_CPU_MMXEXT;
    m_x264Param->cpu |=X264_CPU_SSE;

    m_x264Encoder = x264_encoder_open(m_x264Param);

    m_pictureIn = new x264_picture_t;
    m_pictureIn->i_pts = 0;

    return BLE_SUCESS;
}

QByteArray BleX264Encoder::encode(unsigned char *rgbframe, unsigned long long ts)
{
    unsigned char *src_buf = rgbframe;
    x264_picture_init(m_pictureIn);

    m_pictureIn->img.i_csp = X264_CSP_I420;
    m_pictureIn->img.i_plane = 3;
    m_pictureIn->i_type = X264_TYPE_AUTO;
    m_pictureIn->i_qpplus1 = 0;

    m_pictureIn->i_pts = ts;

    m_pictureIn->img.plane[0] = src_buf;
    m_pictureIn->img.plane[1] = src_buf + m_x264Param->i_height * m_x264Param->i_width;
    m_pictureIn->img.plane[2] = src_buf + m_x264Param->i_height * m_x264Param->i_width * 5 / 4;
    m_pictureIn->img.i_stride[0] = m_x264Param->i_width;
    m_pictureIn->img.i_stride[1] = m_x264Param->i_width >> 1;
    m_pictureIn->img.i_stride[2] = m_x264Param->i_width >> 1;

    x264_picture_t pic_out;
    int nalCount;
    x264_nal_t* nals;
    int len = x264_encoder_encode(m_x264Encoder, &nals, &nalCount, m_pictureIn, &pic_out);

    // TODO delete nals?
    QByteArray ret;
    if (len > 0) {
        ret.append((const char*)nals[0].p_payload, len);
    }

    return ret;
}

void BleX264Encoder::fini()
{
    BleFree(m_pictureIn);
    if (m_x264Encoder) {
        x264_encoder_close(m_x264Encoder);
    }
    BleFree(m_x264Param);
}
