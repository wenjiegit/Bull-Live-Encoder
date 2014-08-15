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

#include "BleCameraSource.hpp"

#include "MOption.h"
#include "BleErrno.hpp"
#include "BleUtil.hpp"
#include "BleLog.hpp"

// opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <QElapsedTimer>

static const int Ble_Camera_Capture_Interval = 50;         // 20fps

BleCameraSource::BleCameraSource(QObject *parent)
    : BleThread(parent)
    , m_interval(Ble_Camera_Capture_Interval)
    , m_cameraIndex(-1)
{
}

BleImage BleCameraSource::getImage()
{
    BleAutoLocker(m_modifyMutex);
    return m_image.clone();
}

void BleCameraSource::stopCapture()
{
    this->stop();
    this->wait();
    this->deleteLater();
}

void BleCameraSource::run()
{
    IplImage *pImg = NULL;
    CvCapture *cap = cvCreateCameraCapture(m_cameraIndex);
    if (!cap) {
        log_error("open camera failed.");
        return;
    }
    log_trace("open camera %d success, name = %s", m_cameraIndex, m_cameraName.toStdString().c_str());

    while (!m_stop) {
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        pImg = cvQueryFrame(cap);
        if (!pImg) continue;

        m_modifyMutex.lock();           // Start lock

        BleImage be;
        be.width = pImg->width;
        be.height = pImg->height;

        be.data = new char[pImg->imageSize];
        memcpy(be.data, pImg->imageData, pImg->imageSize);

        be.dataSize = pImg->imageSize;
        be.format = BleImage_Format_BGR24;

        m_image = be;

        m_modifyMutex.unlock();        // Start unlock


        int elapsedMs = elapsedTimer.elapsed();
        int needSleepMs = m_interval - elapsedMs;
        if (needSleepMs < 0) {
            needSleepMs = 0;
        }
        msleep(needSleepMs);
    }
    cvReleaseCapture(&cap);

    log_trace("BleCameraCapture exit normally.");
}

void BleCameraSource::setCaptureInterval(int interval)
{
    m_interval = interval;
}

void BleCameraSource::setCameraInfo(int index, const QString &name)
{
    m_cameraIndex = index;
    m_cameraName = name;
}
