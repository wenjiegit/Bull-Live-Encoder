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

#include <windows.h>
#include <winuser.h>
#include <qwindowdefs.h>
#include <QGuiApplication>
#include <QScreen>
#include <QImage>
#include <QElapsedTimer>
#include <QCursor>
#include <QPainter>
#include <QtWin>
#include <QBitmap>

#include "BleLog.hpp"
#include "BleUtil.hpp"

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

QString BleWindowsCaptureSource::getSourceName()
{
    return "BleWindowsCaptureSource";
}

QImage BleWindowsCaptureSource::getImage()
{
    BleAutoLocker(m_modifyMutex);
    return m_image;
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

        // TODO make this to option
        // option: could select screen
        QScreen *screen = QGuiApplication::primaryScreen();

        if (screen) {
            QPixmap pixmap = screen->grabWindow(m_wid, m_x, m_y, m_width, m_height);

            // TODO make this to option
#if 1
            QRect desktopRect = QRect(QPoint(0, 0), screen->size());
            if (desktopRect.contains(QCursor::pos())) {
                drawCursor(&pixmap);
            }
#endif
            m_modifyMutex.lock();           // Start lock
            m_image = pixmap.toImage();
            m_modifyMutex.unlock();
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

    // force align to 4 for color cvt performance
    m_width = m_width / 4 *4;
}

void BleWindowsCaptureSource::setCaptureInterval(int interval)
{
    m_interval = interval;
}

void BleWindowsCaptureSource::drawCursor(QPaintDevice *pd)
{
    QPainter p(pd);
    QPixmap pix = cursorPixmap();

    QPoint point = QCursor::pos();
    p.drawPixmap(point.x() - m_x, point.y() - m_y, pix.width(), pix.height(), pix);
}

#ifdef Q_OS_WIN
QPixmap BleWindowsCaptureSource::cursorPixmap()
{
    static HCURSOR cursor = NULL;
    static QPixmap cachedCursor = QPixmap();

    QPixmap cursorPixmap;
    HICON icon;
    CURSORINFO cursorInfo;
    ICONINFO iconInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);

    if(GetCursorInfo(&cursorInfo))
    {
        if (cursor == cursorInfo.hCursor)
            return cachedCursor;

        if (cursorInfo.flags == CURSOR_SHOWING)
        {
            icon = CopyIcon(cursorInfo.hCursor);
            if (GetIconInfo(icon, &iconInfo))
            {
                if (iconInfo.hbmColor != NULL) {
                    cursorPixmap = QtWin::fromHBITMAP(iconInfo.hbmColor, QtWin::HBitmapAlpha);
                } else if (iconInfo.hbmMask != NULL){//if the cursor hasn't color image (for example, Ibeam cursor)
                    cursorPixmap = QtWin::fromHBITMAP(iconInfo.hbmMask, QtWin::HBitmapAlpha).mask();

                    //replace white color with transparent
                    QImage cursorImage = cursorPixmap.copy(0, cursorPixmap.height() / 2, cursorPixmap.width(), cursorPixmap.height() / 2).toImage();
                    cursorImage.setColor(0, Qt::transparent);
                    cursorPixmap = QPixmap::fromImage(cursorImage);
                }
            }
        }
    }

    cursor = cursorInfo.hCursor;
    cachedCursor = cursorPixmap;

    return cursorPixmap;
}
#endif // Q_OS_WIN
