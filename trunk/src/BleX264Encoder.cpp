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

BleX264Encoder::BleX264Encoder()
    : m_x264Param(NULL)
    , m_pictureIn(NULL)
    , m_x264Encoder(NULL)
    , bFirstFrameProcessed(false)
    , frameShift(0)
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
    QString tuneName    = option->option("tune", "x264").toString();
    QString profileName = option->option("profile", "x264").toString();
    int fps = option->option("fps", "encoder").toInt();
    int bps = option->option("bitrate", "encoder").toInt();

    QSize wh = option->option("res", "encoder").toSize();
    int width = wh.width();
    int height = wh.height();

    int maxBitRate = bps;
    int bufferSize = maxBitRate;
    bool bUseCBR = (option->option("BitrateMode", "x264").toString() == "CBR");
    static const float baseCRF = 10.0f;
    int quality = option->option("quality", "x264").toInt();
    int KeyFrameInterval = option->option("KeyFrameInterval", "x264").toInt();

    m_x264Param = new x264_param_t;

    if (tuneName == "Default" || tuneName.isEmpty()) {
        x264_param_default_preset(m_x264Param , presetName.toStdString().c_str(), NULL);
        log_trace("libx264 preset set to %s, tune set to NULL"
                  , presetName.toStdString().c_str());
    } else {
        x264_param_default_preset(m_x264Param , presetName.toStdString().c_str(), tuneName.toStdString().c_str());
        log_trace("libx264 preset set to %s, tune set to %s"\
                  , presetName.toStdString().c_str(), tuneName.toStdString().c_str());
    }

    if (profileName != "Default") {
        x264_param_apply_profile(m_x264Param, profileName.toStdString().c_str());
        log_trace("libx264 profile set to %s", profileName.toStdString().c_str());
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
        m_x264Param->rc.i_rc_method        = X264_RC_CRF; // X264_RC_CRF;
        m_x264Param->rc.f_rf_constant      = baseCRF+float(10-quality);
    }
    //m_x264Param->rc.i_qp_constant = 10;
    //m_x264Param->rc.i_qp_min = 35;

    m_x264Param->b_vfr_input           = 1;
    m_x264Param->i_keyint_max          = fps*KeyFrameInterval;
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
    m_x264Param->b_repeat_headers = 0;
    m_x264Param->b_annexb = 0;

    // TODO make it a option
    m_x264Param->i_bframe = 3;
    m_x264Param->i_threads = 1;

    // cpu capabilities
#if 0
    m_x264Param->cpu = 0;
    m_x264Param->cpu |=X264_CPU_MMX;
    m_x264Param->cpu |=X264_CPU_MMXEXT;
    m_x264Param->cpu |=X264_CPU_SSE;
#endif

    m_x264Encoder = x264_encoder_open(m_x264Param);

    // update video sh
    x264_nal_t *nalOut;
    int nalNum;

    x264_encoder_headers(m_x264Encoder, &nalOut, &nalNum);

    // nalNUm = 3, SPS PPS SEI
    // SPS + PPS
    x264_nal_t &nal = nalOut[0];
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

    x264_nal_t &pps = nalOut[1]; //the PPS always comes after the SPS

    // PPS Serialize
    body.write1Bytes(0x01);
    body.write2Bytes(pps.i_payload - 4);
    body.writeString(MString((char*)pps.p_payload + 4, pps.i_payload - 4));

    appCtx->videoSH = pkt;

    // SEI
    x264_nal_t &seiNal = nalOut[2];
    BleVideoPacket *seiPkt = new BleVideoPacket(Video_Type_H264);
    seiPkt->dts = 0;

    MStream &seiBody = seiPkt->data;
    int skipBytes = 4;
    int newPayloadSize = (seiNal.i_payload - skipBytes);

    unsigned char flshFrameType = 0x17;
    seiBody.write1Bytes(flshFrameType);
    seiBody.write1Bytes(0x01);
    seiBody.write3Bytes(0x00);
    seiBody.write4Bytes(newPayloadSize);
    seiBody.writeString((char*)seiNal.p_payload + skipBytes, newPayloadSize);

    appCtx->seiPkt = seiPkt;

    m_pictureIn = new x264_picture_t;
    m_pictureIn->i_pts = 0;

    return BLE_SUCESS;
}

int BleX264Encoder::encode(unsigned char *rgbframe, mint64 pts)
{
    unsigned char *src_buf = rgbframe;
    x264_picture_init(m_pictureIn);

    m_pictureIn->img.i_csp = X264_CSP_I420;
    m_pictureIn->img.i_plane = 3;
    m_pictureIn->i_type = X264_TYPE_AUTO;
    m_pictureIn->i_qpplus1 = 0;

    m_pictureIn->i_pts = pts;

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

    // @see OBS Encoder_x264.cpp
    //
    int timeOffset = int(picOut.i_pts - picOut.i_dts);

    BleVideoPacket *pkt = dynamic_cast<BleVideoPacket *> (BleAVQueue::instance()->finPkt());
    BleAssert(pkt != NULL);

    // Current BLE only support one nal per frame
    BleAssert(nalNum == 1);

    // log_trace("-----> pts %lld out_pts %lld out_dts %lld diff %d fixed %lld", pts, picOut.i_pts, picOut.i_dts, timeOffset, pkt->dts);

    // NALU payload : 4bytes size + payload
    // NALU payload size : 4bytes size + payload size
    // for b_repeat_headers = 0 in x264_param_t
    // so NALU type is only IDR, SLICE(P or B frame)
    // so you must get SPS PPS before encoding any frame.
    for (int i = 0; i < nalNum; ++i) {
        x264_nal_t &nal = nalOut[i];

        if(nal.i_type == NAL_SLICE_IDR || nal.i_type == NAL_SLICE) {

            // 4 bytes is nalu size
            int skipBytes = 4;
            int newPayloadSize = (nal.i_payload - skipBytes);

            MStream &body = pkt->data;
            unsigned char flshFrameType = ((nal.i_type == NAL_SLICE_IDR) ? 0x17 : 0x27);

            body.write1Bytes(flshFrameType);
            body.write1Bytes(0x01);
            body.write3Bytes(timeOffset);
            body.write4Bytes(newPayloadSize);
            body.writeString((char*)nal.p_payload + skipBytes, newPayloadSize);

        } else
            continue;
    }

    BleAVQueue::instance()->updatePkt(pkt);

    return 0;
}

int BleX264Encoder::getFrameDuration()
{
    MOption *option = MOption::instance();
    int fps = option->option("fps", "encoder").toInt();

    if (fps > 0) {
        return 1000 / fps;
    }

    // 40 : default duration
    return 40;
}

void BleX264Encoder::fini()
{
    BleFree(m_pictureIn);
    if (m_x264Encoder) {
        x264_encoder_close(m_x264Encoder);
    }
    BleFree(m_x264Param);
}
