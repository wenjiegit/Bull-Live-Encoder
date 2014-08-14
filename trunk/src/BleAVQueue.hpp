#ifndef BLEAVQUEUE_HPP
#define BLEAVQUEUE_HPP

#include <QMutex>
#include <QQueue>

#include "BleTimestampBulider.hpp"
#include "BleAVUtil.hpp"

class BleAVQueue
{
public:
    BleAVQueue();

    static BleAVQueue *instance();

    void enqueue(BleAVPacket * pkt);
    QQueue<BleAVPacket *> dequeue();

private:
    BleTimestampBulider m_timestampBulider;
    QMutex m_mutex;
    QQueue<BleAVPacket *> m_queue;
};

#endif // BLEAVQUEUE_HPP
