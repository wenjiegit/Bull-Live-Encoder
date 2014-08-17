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

#include "BleAVQueue.hpp"

#include "BleUtil.hpp"

#include <QDateTime>

qint64 startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

BleAVQueue::BleAVQueue()
{
    m_timestampBulider.setAudioCaptureInternal(23);
    m_timestampBulider.setVideoCaptureInternal(50);
}

BleAVQueue *BleAVQueue::instance()
{
    static BleAVQueue* queue = NULL;
    if (!queue) {
        queue = new BleAVQueue;
    }

    return queue;
}

void BleAVQueue::enqueue(BleAVPacket *pkt)
{
    BleAutoLocker(m_mutex);
    if (m_queue.size() > 300) {
        return;
    }

    if (pkt->pktType == Packet_Type_Audio) {
        pkt->pts = m_timestampBulider.addAudioFrame();
        //pkt->pts = QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime;
    } else if (pkt->pktType == Packet_Type_Video) {
        pkt->pts = m_timestampBulider.addVideoFrame();
    }
    pkt->dts = pkt->pts;

    m_queue.enqueue(pkt);
}

QQueue<BleAVPacket *> BleAVQueue::dequeue()
{
    BleAutoLocker(m_mutex);
    QQueue<BleAVPacket *> ret = m_queue;
    m_queue.clear();

    return ret;
}
