#ifndef BLEAVPACKET_HPP
#define BLEAVPACKET_HPP

#include <QByteArray>

#define Packet_Type_Audio 0x08
#define Packet_Type_Video 0x09

#define Audio_Type_AAC 0x01
#define Audio_Type_MP3 0x02

#define Video_Type_H264 0x17

struct BleAVPacket
{
    BleAVPacket(char type)
        : pktType(type)
        , pts(0)
        , dts(0)
    {

    }

    QByteArray data;
    char pktType;
    qint64 pts;
    qint64 dts;
};

struct BleAudioPacket : public BleAVPacket
{
    BleAudioPacket(char at)
        : BleAVPacket(Packet_Type_Audio)
        , audioType(at)
    {

    }

    char audioType;
};

struct BleVideoPacket : public BleAVPacket
{
    BleVideoPacket(char vt)
        : BleAVPacket(Packet_Type_Video)
        , videoType(vt)
    {

    }

    char videoType;
};

#endif // BLEAVPACKET_HPP
