#include "BleContext.hpp"
#include "BleUtil.hpp"

BleContext::BleContext()
    : videoSH(NULL)
    , audioSH(NULL)
    , seiPkt(NULL)
{
}

void BleContext::setVideoSh(BleAVPacket *pkt)
{
    BleFree(videoSH);
    videoSH = pkt;
}

void BleContext::setAudioSh(BleAVPacket *pkt)
{
    BleFree(audioSH);
    audioSH = pkt;
}

void BleContext::setSei(BleAVPacket *pkt)
{
    BleFree(seiPkt);
    seiPkt = pkt;
}

BleContext *appCtx = new BleContext;
