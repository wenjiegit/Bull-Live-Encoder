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

#include "BleWindowsCaptureSource.hpp"
#include "BleLog.hpp"
#include "BleUtil.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QImage>
#include <QElapsedTimer>
#include <QCursor>
#include <QPainter>

#define Default_Capture_Interval    50      // default is 20 fps

BleWindowsCaptureSource::BleWindowsCaptureSource(QObject *parent)
    : BleThread(parent)
    , m_wid(0)
    , m_x(0)
    , m_y(0)
    , m_width(320)
    , m_height(240)
    , m_interval(Default_Capture_Interval)
{
}

BleWindowsCaptureSource::~BleWindowsCaptureSource()
{

}

BleImage BleWindowsCaptureSource::getImage()
{
    BleAutoLocker(m_modifyMutex);

    BleImage be;
    be = m_image;
    return be;
}

void BleWindowsCaptureSource::stopCapture()
{
    this->stop();
    this->wait();
    this->deleteLater();
}

void BleWindowsCaptureSource::run()
{
    // TODO make could select screen
    // QGuiApplication::screens();
    while (!m_stop) {
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        QScreen *screen = QGuiApplication::primaryScreen();

        if (screen) {
            QPixmap pixmap = screen->grabWindow(m_wid, m_x, m_y, m_width, m_height);
#if 0
            // TODO to draw cursor to image
            drawCursor(&pixmap);
#endif
            QImage image = pixmap.toImage();

            if (image.format() != QImage::Format_RGB888) {
                image = image.convertToFormat(QImage::Format_RGB888);
            }

            m_modifyMutex.lock();           // Start lock

            BleImage be;
            be.width = image.width();
            be.height = image.height();

            be.data = new char[image.byteCount()];
            memcpy(be.data, image.bits(), image.byteCount());

            be.dataSize = image.byteCount();
            be.format = BleImage_Format_RGB24;

            m_image = be;

            m_modifyMutex.unlock();        // Start unlock

        }

        int elapsedMs = elapsedTimer.elapsed();
        int needSleepMs = m_interval - elapsedMs;
        if (needSleepMs < 0) {
            needSleepMs = 0;
        }
        msleep(needSleepMs);
    }

    log_trace("BleWindowsCaptureSource exit normally.");
}

void BleWindowsCaptureSource::setGrabInfo(WId window, int x, int y, int w, int h)
{
    m_wid = window;
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
}

void BleWindowsCaptureSource::setCaptureInterval(int interval)
{
    m_interval = interval;
}

void BleWindowsCaptureSource::drawCursor(QPaintDevice *pd)
{
    QPainter p(pd);
    QPixmap pix = QCursor(Qt::IBeamCursor).pixmap();
    pix.save("xxx.jpg");
    p.drawPixmap(10, 10, pix.width(), pix.height(), pix);
}
