#ifndef BLECONTEXT_HPP
#define BLECONTEXT_HPP

#include "BleAVUtil.hpp"

class BleContext
{
public:
    BleContext();


    BleAVPacket *videoSH;
    BleAVPacket *audioSH;
    BleAVPacket *seiPkt;
};

extern BleContext *appCtx;

#endif // BLECONTEXT_HPP
