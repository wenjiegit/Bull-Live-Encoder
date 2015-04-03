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

#ifndef BleEncoderThread_H
#define BleEncoderThread_H

#include <QThread>
#include <QMutex>
#include <QList>
#include <QImage>
#include <QDateTime>
#include <QWaitCondition>

#include "BleThread.hpp"

class BleX264Encoder;

class BleEncoderThread : public BleThread
{
    Q_OBJECT
public:
    explicit BleEncoderThread(QObject *parent = 0);
    ~BleEncoderThread();

    void init();
    void fini();
    void run();

private:
    BleX264Encoder *m_x264Encoder;
    QThread * m_imageProcessThread;
};

#endif // BleEncoderThread_H
