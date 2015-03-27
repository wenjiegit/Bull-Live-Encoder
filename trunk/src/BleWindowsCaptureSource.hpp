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

#ifndef BLEWINDOWSCAPTURESOURCE_HPP
#define BLEWINDOWSCAPTURESOURCE_HPP

#include <QWidget>
#include <QMutex>
#include <QImage>

#include "BleThread.hpp"
#include "BleSourceAbstract.hpp"

class BleWindowsCaptureSource : public BleThread, public BleSourceAbstract
{
    Q_OBJECT
public:
    explicit BleWindowsCaptureSource(QObject *parent = 0);
    ~BleWindowsCaptureSource();

    virtual QString getSourceName();
    virtual QImage getImage();
    virtual void stopCapture();
    virtual void run();

    void setGrabInfo(WId window = 0, int x = 0, int y = 0, int w = -1, int h = -1);
    virtual void setCaptureInterval(int interval);

private:
    void drawCursor(QPaintDevice *pd);
    QPixmap cursorPixmap();

private:
    WId m_wid;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_interval;

    QImage m_image;
    QMutex m_modifyMutex;
};

#endif // BLEWINDOWSCAPTURESOURCE_HPP
