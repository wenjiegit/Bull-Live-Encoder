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

#include "BleFileSource.hpp"

#include <QDebug>
#include <QElapsedTimer>

// opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "BleUtil.hpp"

BleFileSource::BleFileSource(QObject *parent)
    : BleThread(parent)
    , m_interval(40)
{
}

BleImage BleFileSource::getImage()
{
    BleAutoLocker(m_modifyMutex);
    return m_image.clone();
}

void BleFileSource::stopCapture()
{
    this->stop();
    this->wait();
    this->deleteLater();
}

void BleFileSource::run()
{
    BleAssert(!m_fileName.isEmpty());

    // m_fileName = "rtsp://218.204.223.237:554/live/1/0547424F573B085C/gsfp90ef4k0a6iap.sdp";
    CvCapture* capture = cvCreateFileCapture(m_fileName.toStdString().c_str());
    BleAssert(capture);

    IplImage* frame;
    while(!m_stop)
    {
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        frame = cvQueryFrame(capture);
        if(!frame) break ;

        m_modifyMutex.lock();           // start lock

        BleImage be;
        be.width = frame->width;
        be.height = frame->height;

        be.data = new char[frame->imageSize];
        memcpy(be.data, frame->imageData, frame->imageSize);

        be.dataSize = frame->imageSize;
        be.format = BleImage_Format_BGR24;

        m_image = be;

        m_modifyMutex.unlock();        // end unlock

        int elapsedMs = elapsedTimer.elapsed();
        int needSleepMs = m_interval - elapsedMs;
        if (needSleepMs < 0) {
            needSleepMs = 0;
        }
        msleep(needSleepMs);
    }
    cvReleaseCapture(&capture);

    log_trace("BleFileSource Thread exit normally.");
}

void BleFileSource::setCaptureInterval(int interval)
{
    m_interval = interval;
}

void BleFileSource::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}
