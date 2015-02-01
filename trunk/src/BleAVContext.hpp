#ifndef BLEAVCONTEXT_HPP
#define BLEAVCONTEXT_HPP

#include "BleImageCaptureThread.hpp"

class BleAVContext
{
public:
    BleAVContext();

    static BleAVContext *instance();

    BleImageCaptureThread *captureThread;
};

#endif // BLEAVCONTEXT_HPP
