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

#ifndef BLEAVPACKET_HPP
#define BLEAVPACKET_HPP

#include <QByteArray>
#include "mstream.hpp"

#define Packet_Type_Audio 0x08
#define Packet_Type_Video 0x09

#define Audio_Type_AAC 0x01
#define Audio_Type_MP3 0x02

#define Video_Type_H264 0x17

class BleAVPacket
{
public:
    BleAVPacket(char type)
        : pktType(type)
        , pts(0)
        , dts(0)
        , has_encoded(false)
        , has_captured(false)
    {

    }
    virtual ~BleAVPacket() {}

    MStream data;
    char pktType;
    qint64 pts;
    qint64 dts;
    bool has_encoded;
    bool has_captured;
};

class BleAudioPacket : public BleAVPacket
{
public:
    BleAudioPacket(char at)
        : BleAVPacket(Packet_Type_Audio)
        , audioType(at)
    {

    }
    ~BleAudioPacket() {}

    char audioType;
};

class BleVideoPacket : public BleAVPacket
{
public:
    BleVideoPacket(char vt)
        : BleAVPacket(Packet_Type_Video)
        , videoType(vt)
    {

    }
    ~BleVideoPacket() {}

    char videoType;
};

#endif // BLEAVPACKET_HPP
