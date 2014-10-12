#ifndef BLECONTEXT_HPP
#define BLECONTEXT_HPP

#include "BleAVUtil.hpp"

class BleContext
{
public:
    BleContext();

    void setVideoSh(BleAVPacket *pkt);
    void setAudioSh(BleAVPacket *pkt);
    void setSei(BleAVPacket *pkt);

    inline BleAVPacket *videoSh()   { return videoSH; }
    inline BleAVPacket *audioSh()   { return audioSH; }
    inline BleAVPacket *sei()       { return seiPkt; }

private:
    BleAVPacket *videoSH;
    BleAVPacket *audioSH;
    BleAVPacket *seiPkt;
};

extern BleContext *appCtx;

#endif // BLECONTEXT_HPP
