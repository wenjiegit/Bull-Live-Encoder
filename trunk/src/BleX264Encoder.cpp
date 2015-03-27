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
#include "BleAVUtil.hpp"
#include "BleAVQueue.hpp"
#include "BleContext.hpp"

#include <QSize>
#include <QDebug>

static void x264Log( void *err, int i_level, const char *psz, va_list va)
{
    static char buf[1024];
    vsnprintf(buf, 1024, psz, va);

    log_trace("x264 log, %s", buf);
}

BleX264Encoder::BleX264Encoder()
    : m_x264Param(NULL)
    , m_pictureIn(NULL)
    , m_x264Encoder(NULL)
    , bFirstFrameProcessed(false)
    , frameShift(0)
    , m_encoded_frames(0)
{
}

BleX264Encoder::~BleX264Encoder()
{
    fini();
}

int BleX264Encoder::init()
{
    int ret = BLE_SUCESS;

    MOption *option = MOption::instance();

    QString presetName  = option->option("preset", "x264").toString();
    QString tuneName    = option->option("tune", "x264").toString();
    QString profileName = option->option("profile", "x264").toString();
    int fps = option->option("fps", "encoder").toInt();
    int kbps = option->option("bitrate", "encoder").toInt();

    QSize wh = option->option("res", "encoder").toSize();
    int width = wh.width();
    int height = wh.height();

    int maxBitRate = kbps;
    int bufferSize = maxBitRate;
    bool bUseCBR = (option->option("BitrateMode", "x264").toString() == "CBR");
    int quality = option->option("quality", "x264").toInt();
    int KeyFrameInterval = option->option("KeyFrameInterval", "x264").toInt();
    int threadCount = option->option(Key_Thread_Count, Group_X264).toInt();
    bool enableBFrame = option->option(Key_Enable_B_Frame, Group_X264).toString() == "true" ? true : false;
    int B_frame_count = option->option(Key_B_Frame_Count, Group_X264).toInt();

    m_x264Param = new x264_param_t;

    if (tuneName == "Default" || tuneName.isEmpty()) {
        ret = x264_param_default_preset(m_x264Param , presetName.toStdString().c_str(), NULL);
        if (ret != BLE_SUCESS) {
            log_error("x264_param_default_preset error.");
            return ret;
        } else {
            log_trace("libx264 preset set to %s, tune set to NULL"
                      , presetName.toStdString().c_str());
        }
    } else {
        ret = x264_param_default_preset(m_x264Param , presetName.toStdString().c_str(), tuneName.toStdString().c_str());
        if (ret != BLE_SUCESS) {
            log_error("x264_param_default_preset error.");
            return ret;
        } else {
            log_trace("libx264 preset set to %s, tune set to %s"\
                      , presetName.toStdString().c_str(), tuneName.toStdString().c_str());
        }
    }

    if (profileName != "Default") {
        ret = x264_param_apply_profile(m_x264Param, profileName.toStdString().c_str());
        if (ret != BLE_SUCESS) {
            log_error("x264_param_apply_profile error.");
            return ret;
        } else {
            log_trace("libx264 profile set to %s", profileName.toStdString().c_str());
        }
    } else {
        log_trace("libx264 profile set to Default");
    }

    if(bUseCBR)
    {
        m_x264Param->rc.i_bitrate          = maxBitRate;
        m_x264Param->rc.i_vbv_max_bitrate  = maxBitRate; // vbv-maxrate
        m_x264Param->rc.i_vbv_buffer_size  = bufferSize; // vbv-bufsize
        m_x264Param->i_nal_hrd             = X264_NAL_HRD_CBR;
        m_x264Param->rc.i_rc_method        = X264_RC_ABR;
        m_x264Param->rc.f_rf_constant      = 0.0f;
    }
    else
    {
        m_x264Param->rc.i_vbv_max_bitrate  = maxBitRate;  // vbv-maxrate
        m_x264Param->rc.i_vbv_buffer_size  = bufferSize;  // vbv-bufsize
        m_x264Param->i_nal_hrd             = X264_NAL_HRD_VBR;
        m_x264Param->rc.i_rc_method        = X264_RC_CRF; // X264_RC_CRF;
        m_x264Param->rc.f_rf_constant      = 20.0f + float(10 - quality);

        log_trace("libx264 quality set to %d", quality);
    }

    m_x264Param->b_vfr_input           = 1;
    m_x264Param->i_keyint_max          = fps * KeyFrameInterval;
    m_x264Param->i_width               = width;
    m_x264Param->i_height              = height;
    m_x264Param->vui.b_fullrange       = 0;          //specify full range input levels

    // For some codecs, the time base is closer to the field rate than the frame rate.
    // Most notably, H.264 and MPEG-2 specify time_base as half of frame duration
    // if no telecine is used ...
    // Set to time_base ticks per frame. Default 1, e.g., H.264/MPEG-2 set it to 2.
    // @see ffmpeg: AVodecContex::ticks_per_frame
    // never use timebase = 1000, because vlc will show 1000 fps !!
    int ticks_per_frame = 2;
    m_x264Param->i_timebase_num = 1;
    m_x264Param->i_timebase_den = fps;

    m_x264Param->i_fps_num = m_x264Param->i_timebase_den;
    m_x264Param->i_fps_den = m_x264Param->i_timebase_num * ticks_per_frame;

    // disable start code 00 00 00 01 before NAL
    // instead of nalu size
    m_x264Param->b_repeat_headers = 0;
    m_x264Param->b_annexb = 0;

    m_x264Param->pf_log = x264Log;
    m_x264Param->p_log_private = NULL ;
    m_x264Param->i_log_level = X264_LOG_INFO ;

    // m_x264Param->i_frame_reference = 5;
    if (enableBFrame) {
        m_x264Param->i_bframe = B_frame_count;
        m_x264Param->i_bframe_bias = 0;

        // --b-adapt <integer>     Adaptive B-frame decision method.
        // Higher values may lower threading efficiency.
        //           0: Disabled
        //         - 1: Fast
        //         - 2: Optimal (slow with high --bframes)
        m_x264Param->i_bframe_adaptive = 2;

        // Keep some B-frames as references: 0=off, 1=strict hierarchical, 2=normal
        // take effect only b_frames >= 2
        if (B_frame_count >= 2)
            m_x264Param->i_bframe_pyramid = 1;
    }

    if (threadCount > 0) {
        m_x264Param->i_threads = threadCount;
    }

    // @note
    // never use cpu capabilities.
    // let libx264 to choose.
#if 0
    m_x264Param->cpu = 0;
    m_x264Param->cpu |=X264_CPU_MMX;
    m_x264Param->cpu |=X264_CPU_MMXEXT;
    m_x264Param->cpu |=X264_CPU_SSE;
#endif

    m_x264Encoder = x264_encoder_open(m_x264Param);

    // update video sh
    x264_nal_t *nalOut = NULL;
    int nalNum = 0;
    x264_encoder_headers(m_x264Encoder, &nalOut, &nalNum);

    for (int i = 0; i < nalNum; ++i) {
        x264_nal_t &nal = nalOut[i];
        if (nal.i_type == NAL_SPS) {
            BleVideoPacket *pkt = new BleVideoPacket(Video_Type_H264);
            pkt->dts = 0;

            MStream &body = pkt->data;

            // SPS Serialize
            body.write1Bytes(0x17);
            body.write1Bytes(0x00);
            body.write3Bytes(0x00);
            body.write1Bytes(0x01);
            body.writeString((char*)nal.p_payload + 5, 3);
            body.write1Bytes(0xff);
            body.write1Bytes(0xe1);
            body.write2Bytes(nal.i_payload - 4);
            body.writeString((char*)nal.p_payload + 4, nal.i_payload - 4);

            //the PPS always comes after the SPS
            x264_nal_t &pps = nalOut[++i];

            // PPS Serialize
            body.write1Bytes(0x01);
            body.write2Bytes(pps.i_payload - 4);
            body.writeString(MString((char*)pps.p_payload + 4, pps.i_payload - 4));

            appCtx->setVideoSh(pkt);
        } else if (nal.i_type == NAL_SEI) {
            BleVideoPacket *seiPkt = new BleVideoPacket(Video_Type_H264);
            seiPkt->dts = 0;
            seiPkt->has_encoded = true;

            MStream &seiBody = seiPkt->data;
            int skipBytes = 4;
            int newPayloadSize = (nal.i_payload - skipBytes);

            unsigned char flshFrameType = 0x17;
            seiBody.write1Bytes(flshFrameType);
            seiBody.write1Bytes(0x01);
            seiBody.write3Bytes(0x00);
            seiBody.write4Bytes(newPayloadSize);
            seiBody.writeString((char*)nal.p_payload + skipBytes, newPayloadSize);

            BleAVQueue::instance()->enqueue(seiPkt);
        }
    }

    m_pictureIn = new x264_picture_t;
    m_pictureIn->i_pts = 0;

    return BLE_SUCESS;
}

int BleX264Encoder::encode(unsigned char *rgbframe, mint64 pts, void *opaque)
{
    Q_UNUSED(pts);
    unsigned char *src_buf = rgbframe;
    x264_picture_init(m_pictureIn);

    m_pictureIn->img.i_csp = X264_CSP_I420;
    m_pictureIn->img.i_plane = 3;
    m_pictureIn->i_type = X264_TYPE_AUTO;
    m_pictureIn->i_qpplus1 = 0;

    // @note why i_pts plus 1 everytime
    // because the timebase set as above.
    m_pictureIn->i_pts = ++m_encoded_frames;
    m_pictureIn->opaque = opaque;

    m_pictureIn->img.plane[0] = src_buf;
    m_pictureIn->img.plane[1] = src_buf + m_x264Param->i_height * m_x264Param->i_width;
    m_pictureIn->img.plane[2] = src_buf + m_x264Param->i_height * m_x264Param->i_width * 5 / 4;
    m_pictureIn->img.i_stride[0] = m_x264Param->i_width;
    m_pictureIn->img.i_stride[1] = m_x264Param->i_width >> 1;
    m_pictureIn->img.i_stride[2] = m_x264Param->i_width >> 1;

    x264_picture_t picOut;
    int nalNum;
    x264_nal_t* nalOut;
    int len = x264_encoder_encode(m_x264Encoder, &nalOut, &nalNum, m_pictureIn, &picOut);
    if (len < 0) {
        log_error("x264 encode failed");
        return -1;
    }

    if (nalNum <= 0) {
        log_warn("frame delayed in encoder.");
        return -2;
    }

    if(!bFirstFrameProcessed && nalNum)
    {
        if(picOut.i_dts < 0)
            delayOffset = int(-picOut.i_dts);

        bFirstFrameProcessed = true;
    }

    float timeOffset = float(picOut.i_pts - picOut.i_dts) * BleAVQueue::instance()->timestampBuilder()->videoInternal();

    BleVideoPacket *pkt = dynamic_cast<BleVideoPacket *> (BleAVQueue::instance()->find_unencoded_video());
    BleAssert(pkt != NULL);

    MStream &body = pkt->data;
    unsigned char frameType;
    if (IS_X264_TYPE_I(picOut.i_type)) {
        frameType = 0x17;
    } else {
        frameType = 0x27;
    }

    body.write1Bytes(frameType);
    body.write1Bytes(0x01);
    body.write3Bytes((int)timeOffset);

    // NALU payload : 4bytes size + payload
    // NALU payload size : 4bytes size + payload size
    // for b_repeat_headers = 0 in x264_param_t
    // so NALU type is only IDR, SLICE(P or B frame)
    // so you must get SPS PPS before encoding any frame.
    for (int i = 0; i < nalNum; ++i) {
        x264_nal_t &nal = nalOut[i];
        body.writeString((char*)nal.p_payload, nal.i_payload);
    }

#if 0
    if (IS_X264_TYPE_I(picOut.i_type)) {
        log_trace("I");
    } else if (IS_X264_TYPE_B(picOut.i_type)) {
        log_trace("B");
    } else {
        log_trace("P");
    }
#endif

    BleAVQueue::instance()->update_packet(pkt);

    return 0;
}

float BleX264Encoder::getFrameDuration()
{
    MOption *option = MOption::instance();
    int fps = option->option("fps", "encoder").toInt();

    if (fps > 0) {
        return 1000.0000 / (float)fps;
    }

    // 40 ms : default duration
    return 40.0f;
}

void BleX264Encoder::fini()
{
    BleFree(m_pictureIn);
    if (m_x264Encoder) {
        x264_encoder_close(m_x264Encoder);
    }
    BleFree(m_x264Param);
}
