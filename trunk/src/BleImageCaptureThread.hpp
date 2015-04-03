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

#ifndef BLEIMAGECAPTURETHREAD_HPP
#define BLEIMAGECAPTURETHREAD_HPP

#include "BleThread.hpp"

#include <QWaitCondition>
#include <QMutex>
#include <QQueue>

#include "BleAVQueue.hpp"
#include "BleSourceAbstract.hpp"

class BleImageProcessThread;

class BleImageCaptureThread : public BleThread
{
    Q_OBJECT
public:
    explicit BleImageCaptureThread(QObject *parent = 0);

    void run();

    QQueue<BleImage *> getQueue();

    void fini();

signals:

public slots:

private:
    QWaitCondition m_cond;
    QMutex m_mutex;
    BleImageProcessThread *m_thread;

    double m_pts;
    QQueue<BleImage *> m_queue;
};

#endif // BLEIMAGECAPTURETHREAD_HPP
