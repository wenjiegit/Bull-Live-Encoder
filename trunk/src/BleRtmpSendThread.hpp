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

#ifndef BLERTMPSENDTHREAD_H
#define BLERTMPSENDTHREAD_H

#include "BleThread.hpp"

#include <QMutex>
#include <QTimer>
#include <QList>
#include <QElapsedTimer>

class BleRtmpMuxer;

class BleRtmpSendThread : public BleThread
{
    Q_OBJECT
public:
    BleRtmpSendThread(QObject * parent = 0);
    ~BleRtmpSendThread();

    virtual void run();

private:
#ifdef Q_OS_WIN
    int wsaStart();
#endif
    int service(BleRtmpMuxer & muxer);

private:
    int sendVideoSh(BleRtmpMuxer & muxer);
    int sendAudioSh(BleRtmpMuxer & muxer);
    int sendMetadata(BleRtmpMuxer &muxer);

private slots:
    void onTimeout();

private:
    int m_audio_send_bytes;
    int m_video_send_bytes;
    int m_fps;
    qint64 m_data_send_bytes;
    QMutex m_mutex;
    QTimer m_timer;

    struct kbps
    {
        float audio_avg;
        float video_avg;
        float fps;
    };

    QList<kbps> m_kbps;
    QElapsedTimer m_elapsed_timer;

signals:
    void status(int audioKbps, int videoKbps, int fps, qint64 sendDataCount);
};

#endif // BLERTMPSENDTHREAD_H
