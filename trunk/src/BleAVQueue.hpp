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

#ifndef BLEAVQUEUE_HPP
#define BLEAVQUEUE_HPP

#include <QMutex>
#include <QQueue>
#include <QHash>

#include "BleTimestampBulider.hpp"
#include "BleAVUtil.hpp"

class BleAVQueue
{
public:
    BleAVQueue();
    ~BleAVQueue();

    static BleAVQueue *instance();
    static void destroy();

    void init();

    void enqueue(BleAVPacket * pkt);

    BleAVPacket * find_unencoded_video();
    BleAVPacket *find_uncaptured_video();

    void update_packet(BleAVPacket * pkt);

    QQueue<BleAVPacket *> dequeue();

    inline BleTimestampBulider *timestampBuilder() { return m_timestampBulider; }

private:
    void fini();

private:
    BleTimestampBulider *m_timestampBulider;
    QMutex m_mutex;
    QList<BleAVPacket *> m_queue;
};

#endif // BLEAVQUEUE_HPP
