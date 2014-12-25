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

#ifndef BLEAUDIOENCODERABSTRACT_HPP
#define BLEAUDIOENCODERABSTRACT_HPP

#include <QByteArray>

class BleAudioEncoderAbstract
{
public:
    enum AudioEncoderType
    {
        AAC,
        MP3
    };

public:
    BleAudioEncoderAbstract();
    virtual ~BleAudioEncoderAbstract();

    virtual bool init(int samplerate, int channel, int bitrate) = 0;
    virtual void fini() = 0;
    virtual bool encode(const QByteArray &data, QByteArray &output) = 0;
    virtual int getFrameSize() = 0;
    virtual float getFrameDuration() = 0;
    inline AudioEncoderType encoderType() { return m_encoderType; }

protected:
    int m_samplerate;
    int m_channels;
    int m_bitrate;
    AudioEncoderType m_encoderType;
};

#endif // BLEAUDIOENCODERABSTRACT_HPP
