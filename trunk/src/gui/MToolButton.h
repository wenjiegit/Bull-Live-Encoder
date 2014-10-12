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

#ifndef MToolButton_H
#define MToolButton_H

#include <QToolButton>
#include <QPersistentModelIndex>
#include <QTimer>
#include <QTimeLine>

enum{
    STATUS_NORMAL,
    STATUS_HOVE,
    STATUS_PRESS,
    STATUS_DISABLED
};

class MToolButton : public QToolButton
{
  Q_OBJECT
public:
    explicit MToolButton(QWidget * parent=0);
    ~MToolButton();

    void setPixmap(const QPixmap &pixmap);
    inline void setColor(const QColor& color)
    {
        m_fontColor = color;
        update();
    }

    void setButtonStatus(int status);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private slots:
    void frameChanged ( int frame );
    void finished ();

private:
    QPixmap m_normal;
    QPixmap m_hover;
    QPixmap m_pressed;
    QPixmap m_disabled;
    QImage resultImage;
    QImage sourceImage;
    int alpha;
    int status;
    QColor m_fontColor;

protected:
    QTimeLine timeLine;
};

#endif // MToolButton_H
