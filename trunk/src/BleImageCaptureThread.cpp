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

BleImageCaptureThread::BleImageCaptureThread(QObject *parent) :
    BleThread(parent)
{
}

void BleImageCaptureThread::capture(double pts)
{
    m_mutex.lock();
    m_pts = pts;
    m_mutex.unlock();
    m_cond.wakeOne();
}

void BleImageCaptureThread::run()
{
    while (!m_stop) {
        m_mutex.lock();
        m_cond.wait(&m_mutex);

        // capture
        BleImage *image = m_thread->getImage();
        image->pts = m_pts;
        m_queue.append(image);

        m_mutex.unlock();
    }
}

void BleImageCaptureThread::setImageProcessThread(BleImageProcessThread *thread)
{
    m_thread = thread;
}

QQueue<BleImage *> BleImageCaptureThread::getQueue()
{
    m_mutex.lock();

    QQueue<BleImage *> ret = m_queue;
    m_queue.clear();

    m_mutex.unlock();

    return ret;
}
