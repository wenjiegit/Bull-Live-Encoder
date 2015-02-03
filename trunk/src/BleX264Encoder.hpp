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

#ifndef BLEX264ENCODER_H
#define BLEX264ENCODER_H

#include <stdint.h>
extern "C"{
#include "x264.h"
}

#include <QObject>
#include <QList>

#include "mstream.hpp"

class BleVideoPacket;

class BleX264Encoder
{
public:
    BleX264Encoder();
    ~BleX264Encoder();

    int init();
    int encode(unsigned char *rgbframe, mint64 pts, void *opaque);

    float getFrameDuration();

private:
    void fini();

private:
    x264_param_t * m_x264Param;
    x264_picture_t *m_pictureIn;
    x264_t *m_x264Encoder;
    int delayOffset;
    bool bFirstFrameProcessed;
    int frameShift;
    int m_encoded_frames;
};

#endif // BLEX264ENCODER_H
