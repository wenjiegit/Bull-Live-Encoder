#include "BleContext.hpp"

BleContext::BleContext()
    : videoSH(NULL)
    , audioSH(NULL)
    , seiPkt(NULL)
{
}

BleContext *appCtx = new BleContext;
