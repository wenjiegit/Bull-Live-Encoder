#include "BleAVContext.hpp"

BleAVContext::BleAVContext()
{
    captureThread = 0;
}

BleAVContext *BleAVContext::instance()
{
    static BleAVContext *ret = 0;
    if (!ret) {
        ret = new BleAVContext;
    }

    return ret;
}
