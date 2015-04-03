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

#include "BleImageCaptureThread.hpp"
#include "BleImageProcessThread.hpp"
#include "BleLog.hpp"
#include "BleAVQueue.hpp"
#include "BleUtil.hpp"
#include "BleSceneWidget.hpp"

BleImageCaptureThread::BleImageCaptureThread(QObject *parent) :
    BleThread(parent)
{

}

void BleImageCaptureThread::run()
{
    while (!m_stop) {
        BleAVPacket *pkt = BleAVQueue::instance()->find_uncaptured_video();
        if (!pkt) {
            msleep(5);
            continue;
        }

        // capture
        BleImageProcessThread *thread = BleSceneWidget::instance()->currentImageProcessThread();
        BleImage *image = thread->getImage();
        Q_ASSERT(image);
        image->pts = pkt->dts;
        pkt->has_captured = true;
        image->opaque = pkt;

        m_mutex.lock();
        m_queue.append(image);
        m_mutex.unlock();
    }
}

QQueue<BleImage *> BleImageCaptureThread::getQueue()
{
    BleAutoLocker(m_mutex);

    QQueue<BleImage *> ret = m_queue;
    m_queue.clear();

    return ret;
}

void BleImageCaptureThread::fini()
{
    BleAutoLocker(m_mutex);
    while (!m_queue.isEmpty()) {
        BleImage *image = m_queue.dequeue();
        BleFree(image);
    }
}
