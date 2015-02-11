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

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

#include "BleRtmpMuxer.hpp"
#include "rtmp.h"
#include "mstring.hpp"
#include "mstringlist.hpp"
#include "BleVersion.hpp"

#define NALU_TYPE_SLICE     1
#define NALU_TYPE_DPA       2
#define NALU_TYPE_DPB       3
#define NALU_TYPE_DPC       4
#define NALU_TYPE_IDR       5
#define NALU_TYPE_SEI       6
#define NALU_TYPE_SPS       7
#define NALU_TYPE_PPS       8
#define NALU_TYPE_AUD       9
#define NALU_TYPE_EOSEQ     10
#define NALU_TYPE_EOSTREAM  11
#define NALU_TYPE_FILL      12

#define StreamChannel_Metadata  0x03
#define StreamChannel_Video     0x04
#define StreamChannel_Audio     0x05

static char * put_byte(char *output, uint8_t nVal)
{
    output[0] = nVal;

    return output + 1;
}

static char * put_be16(char *output, uint16_t nVal)
{
    output[1] = nVal & 0xff;
    output[0] = nVal >> 8;

    return output + 2;
}

#if 0
static char * put_be24(char *output, uint32_t nVal)
{
    output[2] = nVal & 0xff;
    output[1] = nVal >> 8;
    output[0] = nVal >> 16;

    return output + 3;
}
#endif

#if 0
static char * put_be32(char *output, uint32_t nVal)
{
    output[3] = nVal & 0xff;
    output[2] = nVal >> 8;
    output[1] = nVal >> 16;
    output[0] = nVal >> 24;

    return output + 4;
}
#endif

#if 0
static char * put_be64(char *output, uint64_t nVal)
{
    output = put_be32(output, nVal >> 32);
    output = put_be32(output, nVal);

    return output;
}
#endif

static char * put_amf_string(char *c, const char *str)
{
    uint16_t len = strlen(str);
    c = put_be16(c, len);
    memcpy(c, str, len);

    return c + len;
}

static char * put_amf_double(char *c, double d)
{
    // type: Number
    *c++ = AMF_NUMBER;
    {
        unsigned char *ci, *co;
        ci = (unsigned char *) &d;
        co = (unsigned char *) c;
        co[0] = ci[7];
        co[1] = ci[6];
        co[2] = ci[5];
        co[3] = ci[4];
        co[4] = ci[3];
        co[5] = ci[2];
        co[6] = ci[1];
        co[7] = ci[0];
    }

    return c + 8;
}

FlvMetaData::FlvMetaData()
{
    audiocodecid = CODECID_AAC;
    videocodecid = CODECID_H264;
}

/*!
    @brief RtmpAU rtmp access unit
*/
class RtmpAU
{
public:
    RtmpAU(const string& url);
    ~RtmpAU();

    int connect();
    int close();
    void close_socket();
    int sendPacket(const string &data, unsigned long long timestamp, unsigned int pktType, int channel);

private:
    string m_rtmpUrl;
    RTMP* m_pRtmp;
};

BleRtmpMuxer::BleRtmpMuxer()
{
}

int BleRtmpMuxer::addH264(const MStream &data, unsigned long long dts)
{
    return m_rtmpAU->sendPacket(data, dts, RTMP_PACKET_TYPE_VIDEO, StreamChannel_Video);
}

int BleRtmpMuxer::addAAC(const MStream &data, unsigned long long dts)
{
    return m_rtmpAU->sendPacket(data, dts, RTMP_PACKET_TYPE_AUDIO, StreamChannel_Audio);
}

int BleRtmpMuxer::setMetaData(const FlvMetaData &metaData, MStream &body_data)
{
    m_metaData.clear();

    char body[1024] = { 0 };
    char * p = (char *) body;

    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "@setDataFrame");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "onMetaData");
    p = put_byte(p, AMF_OBJECT);
    p = put_amf_string(p, "encoder");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, BLE_NAME);

    p = put_amf_string(p, "encoder_authors");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, BLE_AUTHORS);

    p = put_amf_string(p, "width");
    p = put_amf_double(p, metaData.width);

    p = put_amf_string(p, "height");
    p = put_amf_double(p, metaData.height);

    p = put_amf_string(p, "framerate");
    p = put_amf_double(p, metaData.framerate);

    p = put_amf_string(p, "videocodecid");
    p = put_amf_double(p, metaData.videocodecid);

    p = put_amf_string(p, "audiodatarate");
    p = put_amf_double(p, metaData.audiodatarate);

    p = put_amf_string(p, "audiosamplerate");
    p = put_amf_double(p, metaData.audiosamplerate);

    p = put_amf_string(p, "audiosamplesize");
    p = put_amf_double(p, metaData.audiosamplesize);

    p = put_amf_string(p, "audiocodecid");
    p = put_amf_double(p, metaData.audiocodecid);

    p = put_amf_string(p, "audiochannels");
    p = put_amf_double(p, metaData.stereo ? 2 : 1);

    p = put_amf_string(p, "creationdate");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, metaData.creationdate.c_str());

    p = put_amf_string(p, "canSeekToEnd");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "false");

    p = put_amf_string(p, "");
    p = put_byte(p, AMF_OBJECT_END);

    m_metaData = string(body, p - body);
    body_data.append(m_metaData);

    return m_rtmpAU->sendPacket(m_metaData, 0, RTMP_PACKET_TYPE_INFO, StreamChannel_Metadata);
}

void BleRtmpMuxer::setRtmpUrl(const string &url)
{
    m_rtmpUrl = url;
}

int BleRtmpMuxer::start()
{
    m_rtmpAU = new RtmpAU(m_rtmpUrl);
    
    return m_rtmpAU->connect();
}

int BleRtmpMuxer::stop()
{
    return m_rtmpAU->close();
}

void BleRtmpMuxer::close_socket()
{
    m_rtmpAU->close_socket();
}

string BleRtmpMuxer::genSequenceHeader()
{
    string seq;

    seq.append(1, 0x17);                // 1:keyframe  7:AVC
    seq.append(1, 0x00);                // 0 : AVC sequence header
    seq.append(1, 0x00);                // followings 3 bytes is cts, for B frame
    seq.append(1, 0x00);
    seq.append(1, 0x00);

    // AVCDecoderConfigurationRecord.
    seq.append(1, 0x01);                // configurationVersion
    seq.append(1, m_cachedSPS.at(1));   // AVCProfileIndication
    seq.append(1, m_cachedSPS.at(2));   // profile_compatibility
    seq.append(1, m_cachedSPS.at(3));   // AVCLevelIndication
    seq.append(1, 0xFF);                // lengthSizeMinusOne

    seq.append(1, 0xE1);                // = 0xe1 & 0x1f

    // sps data length
    unsigned short sps_len = m_cachedSPS.size();
    seq.append(1, sps_len >> 8);
    seq.append(1, sps_len & 0xff);
    seq.append(m_cachedSPS);            // sps data

    seq.append(1, 0x01);                // pps data length


    unsigned short pps_len = m_cachedPPS.size();
    seq.append(1, pps_len >> 8);
    seq.append(1, pps_len & 0xff);
    seq.append(m_cachedPPS);

    return seq;
}

string BleRtmpMuxer::genVideoFrame(char *data, int size, unsigned long long pts, unsigned long long dts, bool keyframe)
{
    string frame;

    if (keyframe) {
        frame.append(1, 0x17);
    } else {
        frame.append(1, 0x27);
    }

    int cts = pts - dts;
    char *pcts = (char *)&cts;

    frame.append(1, 0x01);
    frame.append(1, pcts[2]);
    frame.append(1, pcts[1]);
    frame.append(1, pcts[0]);

    // nalu size
    frame.append(1, size >> 24);
    frame.append(1, size >> 16);
    frame.append(1, size >> 8);
    frame.append(1, size & 0xff);

    frame.append(data, size);

    return frame;
}

string BleRtmpMuxer::genAudioFrame(char *data, int size, bool sequence)
{
    string frame;
    frame.append(1, 0xAF);
    if (sequence) {
        frame.append(1, 0x00);
    } else {
        frame.append(1, 0x01);
    }

    frame.append(data, size);

    return frame;
}

int BleRtmpMuxer::addH264Internal(char *data, int size, unsigned long long pts, unsigned long long dts)
{
    string pkt;
    char *h264Data = (char*)(data);
    int naluType = data[0] & 0x1f;

    switch (naluType) {
    case NALU_TYPE_SPS:
        m_cachedSPS = string(h264Data, size);
        return TRUE;

    case NALU_TYPE_PPS:
        {
            m_cachedPPS = string(h264Data, size);
            string seq = genSequenceHeader();
            pkt.append(seq);
        }
        break;
    case NALU_TYPE_IDR:
        {
            string idr = genVideoFrame(h264Data, size, pts, dts, true);
            pkt.append(idr);
        }
        break;
    default:
        {
            string frame = genVideoFrame(h264Data, size, pts, dts, false);
            pkt.append(frame);
        }
        break;
    }

    return m_rtmpAU->sendPacket(pkt, pts, RTMP_PACKET_TYPE_VIDEO, StreamChannel_Video);
}

int BleRtmpMuxer::addAACInternal(char *data, int size, unsigned long long pts)
{
    string pkt = genAudioFrame(data, size, false);
    
    return m_rtmpAU->sendPacket(pkt, pts, RTMP_PACKET_TYPE_AUDIO, StreamChannel_Audio);
}

RtmpAU::RtmpAU(const string &url)
    : m_rtmpUrl(url)
    , m_pRtmp(RTMP_Alloc())
{
    RTMP_Init(m_pRtmp);
    m_pRtmp->m_bUseNagle = TRUE;

    const char *av_val = "FMLE/3.0 (compatible; FMSc/1.0)";
    m_pRtmp->Link.flashVer.av_val = const_cast<char *> (av_val);
    m_pRtmp->Link.flashVer.av_len = (int)strlen(m_pRtmp->Link.flashVer.av_val);

    m_pRtmp->m_outChunkSize = 4096;//RTMP_DEFAULT_CHUNKSIZE;//
    m_pRtmp->m_bSendChunkSizeInfo = TRUE;
}

RtmpAU::~RtmpAU()
{
}

int RtmpAU::connect()
{
    char *url = const_cast<char*>(m_rtmpUrl.c_str());

    int ret = TRUE;
    if ((ret = RTMP_SetupURL(m_pRtmp, url)) != TRUE) {
        return ret;
    }

    RTMP_EnableWrite(m_pRtmp);

    if ((ret = RTMP_Connect(m_pRtmp, NULL)) != TRUE) {
        return ret;
    }

    if ((ret = RTMP_ConnectStream(m_pRtmp, 0)) != TRUE) {
        return ret;
    }

    int tcpBufferSize = 8196*4;
    int curTCPBufSize, curTCPBufSizeSize = 4;
    getsockopt (m_pRtmp->m_sb.sb_socket, SOL_SOCKET, SO_SNDBUF, (char *)&curTCPBufSize, &curTCPBufSizeSize);

    if(curTCPBufSize < int(tcpBufferSize))
    {
        setsockopt (m_pRtmp->m_sb.sb_socket, SOL_SOCKET, SO_SNDBUF, (const char *)&tcpBufferSize, sizeof(tcpBufferSize));
        getsockopt (m_pRtmp->m_sb.sb_socket, SOL_SOCKET, SO_SNDBUF, (char *)&curTCPBufSize, &curTCPBufSizeSize);
    }

    return ret;
}

int RtmpAU::close()
{
    if (m_pRtmp) {
        RTMP_Close(m_pRtmp);
        RTMP_Free(m_pRtmp);

        m_pRtmp = NULL;
    }

    return 0;
}
#include <winsock.h>
#include "BleLog.hpp"
void RtmpAU::close_socket()
{
    if (m_pRtmp) {
        SOCKET so = m_pRtmp->m_sb.sb_socket;
        if (so > 0) {
            ::shutdown(so, 2);
#if defined (Q_OS_WIN)
            ::closesocket(so);
#elif defined (Q_OS_LINUX)
            ::close(so);
#endif
        }
    }
}

int RtmpAU::sendPacket(const string &data, unsigned long long timestamp, unsigned int pktType, int channel)
{
    if (m_pRtmp == NULL) {
        return FALSE;
    }

    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, data.size());

    packet.m_packetType = pktType;
    packet.m_nChannel = channel;
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet.m_nTimeStamp = timestamp;
    packet.m_hasAbsTimestamp = 0;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;
    packet.m_nBodySize = data.size();
    memcpy(packet.m_body, data.data(), data.size());

    int ret = RTMP_SendPacket(m_pRtmp, &packet, 0);
    RTMPPacket_Free(&packet);

    if (ret != TRUE)
    {
        perror("RTMP_SendPacket error");
        return ret;
    }

    return TRUE;
}
