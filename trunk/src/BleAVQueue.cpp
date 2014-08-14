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
