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

static BleAVQueue* gs_queue = NULL;

BleAVQueue::BleAVQueue()
{
    m_timestampBulider = new BleTimestampBulider;
    m_timestampBulider->setAudioCaptureInternal(23);
    m_timestampBulider->setVideoCaptureInternal(50);
}

BleAVQueue::~BleAVQueue()
{
    BleFree(m_timestampBulider);
}

BleAVQueue *BleAVQueue::instance()
{
    if (!gs_queue) {
        gs_queue = new BleAVQueue;
    }

    return gs_queue;
}

void BleAVQueue::destroy()
{
    if (gs_queue)
        gs_queue->fini();

    BleFree(gs_queue);
}

void BleAVQueue::enqueue(BleAVPacket *pkt)
{
    BleAutoLocker(m_mutex);

    m_queue << pkt;
}

BleAVPacket *BleAVQueue::find_unencoded_video()
{
    BleAutoLocker(m_mutex);
    // find first un-ready video
    for (int i = 0; i < m_queue.size(); ++i) {
        BleAVPacket *pkt = m_queue.at(i);
        if (pkt->pktType == Packet_Type_Video && !pkt->has_encoded) return pkt;
    }

    return NULL;
}

BleAVPacket *BleAVQueue::find_uncaptured_video()
{
    BleAutoLocker(m_mutex);
    for (int i = 0; i < m_queue.size(); ++i) {
        BleAVPacket *pkt = m_queue.at(i);
        if (pkt->pktType == Packet_Type_Video && !pkt->has_captured && !pkt->has_encoded) return pkt;
    }

    return NULL;
}

void BleAVQueue::update_packet(BleAVPacket *pkt)
{
    BleAutoLocker(m_mutex);

    pkt->has_encoded = true;
}

QQueue<BleAVPacket *> BleAVQueue::dequeue()
{
    BleAutoLocker(m_mutex);

    QQueue<BleAVPacket *> pkts;
    while (!m_queue.empty()) {
        BleAVPacket *pkt = m_queue.first();
        if (pkt->has_encoded) {
            pkts << pkt;
            m_queue.removeFirst();
        } else
            break;
    }

    return pkts;
}

void BleAVQueue::fini()
{
    BleAutoLocker(m_mutex);

    for (int i = 0; i < m_queue.size(); ++i) {
        BleAVPacket *pkt = m_queue.at(i);
        BleFree(pkt);
    }
}
