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

    // just enqueue
    m_queue << pkt;
}

BleAVPacket *BleAVQueue::finPkt()
{
    // find first un-ready video
    for (int i = 0; i < m_queue.size(); ++i) {
        BleAVPacket *pkt = m_queue.at(i);
        if (!pkt->ready) return pkt;
    }

    return NULL;
}

void BleAVQueue::updatePkt(BleAVPacket *pkt)
{
    BleAutoLocker(m_mutex);

    pkt->ready = true;
}

QQueue<BleAVPacket *> BleAVQueue::dequeue()
{
    BleAutoLocker(m_mutex);

    QQueue<BleAVPacket *> pkts;
    while (!m_queue.empty()) {
        BleAVPacket *pkt = m_queue.first();
        if (pkt->ready) {
            pkts << pkt;

            // erase from m_queue
            m_queue.removeFirst();
        } else
            break;
    }

    return pkts;
}
