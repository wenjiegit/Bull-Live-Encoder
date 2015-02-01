#ifndef BLEAVCONTEXT_HPP
#define BLEAVCONTEXT_HPP

class BleImageCaptureThread;

class BleAVContext
{
public:
    BleAVContext();

    BleImageCaptureThread *captureThread;
};

#endif // BLEAVCONTEXT_HPP
