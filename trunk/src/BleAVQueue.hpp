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

    inline BleTimestampBulider *timestampBuilder();

private:
    BleTimestampBulider m_timestampBulider;
    QMutex m_mutex;
    QQueue<BleAVPacket *> m_queue;
};

// inline function
BleTimestampBulider *BleAVQueue::timestampBuilder()
{
    return &m_timestampBulider;
}

#endif // BLEAVQUEUE_HPP
