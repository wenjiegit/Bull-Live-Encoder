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

#ifndef QJTTABBAR_H
#define QJTTABBAR_H

#include <QWidget>
#include "gui_global.h"

class QPainter;
class QMouseEvent;

class GUISHARED_EXPORT QjtTabBar : public QWidget
{
    Q_OBJECT
    
public:
    QjtTabBar(QWidget *parent = 0);
    ~QjtTabBar();
    void addItem(const QString &text);
    void clearItems();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void leaveEvent(QEvent *);
    int mouseRange(QMouseEvent *,int);
    void drawColor(QPainter *painter,const QRect &rect,int type);

private:
    int m_winHeight;
    int m_winWidth;
    int m_rightLeftDistance;
    int m_topDownDistance;
    int m_selectItemNum;
    int m_moveItemNum;

    QStringList m_texts;
    QList<QRegion> m_Regions;

signals:
    void currentChanged(int);
};

#endif // QJTTABBAR_H
