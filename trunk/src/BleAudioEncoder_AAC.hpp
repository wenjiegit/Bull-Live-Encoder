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

#ifndef BLEAUDIOENCODER_AAC_HPP
#define BLEAUDIOENCODER_AAC_HPP

#include <QByteArray>

#include "faac.h"
#include "BleAudioEncoderAbstract.hpp"

class BleAudioEncoder_AAC : public BleAudioEncoderAbstract
{
public:
    BleAudioEncoder_AAC();

    virtual bool init(int samplerate, int channel, int bitrate);
    virtual void fini();
    virtual bool encode(const QByteArray &data, QByteArray &output);\
    virtual int getFrameSize();
    virtual float getFrameDuration();

    QByteArray getHeader();

public:
    //int sample_rate;
    int m_nFrameSize;
    int m_channels;

    unsigned long m_samplesInputSize;
    unsigned long m_maxOutputSize;

    faacEncHandle   m_faacHandle;
    int*            m_pInBuf;
    unsigned char *m_outputBuffer;
    QByteArray m_header;
};

#endif // BLEAUDIOENCODER_AAC_HPP
