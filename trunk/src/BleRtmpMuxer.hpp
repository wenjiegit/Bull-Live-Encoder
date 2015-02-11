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

#ifndef BLERTMPMUXER_H
#define BLERTMPMUXER_H

#include <string>

#include "mstream.hpp"

using namespace std;

#define CODECID_H264    (0x07)
#define CODECID_AAC     (0x0a)

/*!
    @brief the flv metadata
*/
struct FlvMetaData
{
    int audiocodecid;
    int audiodatarate;
    int audiodelay;
    int audiosamplerate;
    int audiosamplesize;
    bool canSeekToEnd;
    string creationdate;
    int duration;
    int filesize;
    int framerate;
    int height;
    bool stereo;
    int videocodecid;
    int videodatarate;
    int width;

    FlvMetaData();
};

class RtmpAU;

/*!
    @brief Rtmp Muxer
*/
class BleRtmpMuxer
{
public:
    BleRtmpMuxer();

    /*!
        @param data point to one NALU
    */
    int addH264(const MStream &data, unsigned long long dts);

    /*!
        @param data point to one audio frame
    */
    int addAAC(const MStream &data, unsigned long long dts);

    int setMetaData(const FlvMetaData &metaData, MStream &body_data);

    /*!
        @param url a complete RTMP URL
    */
    void setRtmpUrl(const string &url);

    /*!
        @brief do following:

        1. connect to server
        2. gen metadata
    */
    int start();

     /*!
        @brief stop muxer.
                       
      */
    int stop();

    /*!
        close under-layer socket.
    */
    void close_socket();

private:
    string genSequenceHeader();
    string genSPS();
    string genVideoFrame(char *data, int size, unsigned long long pts, unsigned long long dts, bool keyframe);
    string genAudioFrame(char *data, int size, bool sequence);
    int addH264Internal(char *data, int size, unsigned long long pts, unsigned long long dts);
    int addAACInternal(char *data, int size, unsigned long long pts);

private:
    RtmpAU *m_rtmpAU;
    string m_rtmpUrl;
    string m_metaData;
    string m_cachedSPS;
    string m_cachedPPS;
};

#define TRUE 1
#define FALSE 0

#endif // BLERTMPMUXER_H
