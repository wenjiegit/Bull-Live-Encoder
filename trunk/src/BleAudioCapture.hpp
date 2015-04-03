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

#ifndef BLEAUDIOCAPTURE_HPP
#define BLEAUDIOCAPTURE_HPP

#include <QHash>
#include <QMutex>
#include <QWaitCondition>

#include "BleThread.hpp"
#include "RtAudio.h"

class BleAudioEncoderAbstract;

class BleAudioCapture : public BleThread
{
    Q_OBJECT
public:
    BleAudioCapture(QObject * parent = 0);
    ~BleAudioCapture();

    virtual void run();

    static QHash<int, QString> availableDevices();

private:
    void onDataCaptured(char *data, int size);
    void audioMix(QByteArray &a1, QByteArray &a2);

private:
    RtAudio *m_grabEngine;
    BleAudioEncoderAbstract *m_audioEncoder;
    QMutex m_mutex;

    QByteArray m_bytesCache;
    bool m_hasSendHeader;
    int m_bitDepth;
    int m_sampleRate;
    int m_channels;
    int m_bitrate;

public:
    friend int handleAudioData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                          double streamTime, RtAudioStreamStatus status, void *data);

    int startCapture(int bitrate = 96000, int sampleRate = 44100, int channels = 2, int deviceID = -1);
    int stopCapture();
};

#endif // BLEAUDIOCAPTURE_HPP
