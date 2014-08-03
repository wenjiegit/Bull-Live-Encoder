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

#include "QjtTabBar.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QDebug>

const int isNull = -1;

const int mousePressEventType = 0;
const int mouseMoveEventType = 1;

QjtTabBar::QjtTabBar(QWidget *parent)
    : QWidget(parent),
      m_winHeight(110),
      m_winWidth(30),
      m_rightLeftDistance(30),
      m_topDownDistance(15),
      m_selectItemNum(0),
      m_moveItemNum(0)
{
    setMouseTracking(true);
    setFixedHeight(m_winHeight);
}

QjtTabBar::~QjtTabBar()
{
    
}

void QjtTabBar::addItem(const QString &text)
{
    m_texts << text;
}

void QjtTabBar::clearItems()
{
    m_texts.clear();
    m_Regions.clear();
}

void QjtTabBar::mousePressEvent(QMouseEvent *e)
{
    int currentChangedNum = mouseRange(e, mousePressEventType);
    if(currentChangedNum != isNull)
        emit currentChanged(currentChangedNum);
    update();
}

void QjtTabBar::mouseMoveEvent(QMouseEvent *e)
{
    mouseRange(e, mouseMoveEventType);
    update();
}

int QjtTabBar::mouseRange(QMouseEvent *e, int type)
{
    QRect rect(e->pos().x(),e->pos().y(),1,1);
    for(int i = 0; i < m_Regions.size(); ++i)
    {
        QRegion region = m_Regions.value(i);
        if (!region.intersected(rect).isEmpty())
        {
            if(type == mousePressEventType)
                m_selectItemNum = i;
            else
                m_moveItemNum = i;
            return i;
        }
    }
    return isNull;
}

void QjtTabBar::leaveEvent(QEvent *)
{
    m_moveItemNum = m_selectItemNum;
    update();
}

void QjtTabBar::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    if(m_texts.isEmpty())
        return;
    for(int i = 0; i < m_texts.size(); ++i)
    {
        QFontMetrics fm(this->font());
        int textWidth = fm.width(m_texts.value(i));
        int textItemWidth = textWidth + m_rightLeftDistance * 2;
        if(textItemWidth > m_winWidth)
            m_winWidth = textItemWidth;
    }
    QFontMetrics fm(this->font());
    m_winHeight = fm.height() + m_topDownDistance *2;
    setFixedHeight(m_winHeight);
    QPainter painter(this);
    m_Regions.clear();
    for(int i = 0; i < m_texts.size(); ++i)
    {
        QRect rect(i * m_winWidth,0,m_winWidth,m_winHeight);
        if(i == m_moveItemNum)
        {
            if(m_moveItemNum != m_selectItemNum)
            {
                drawColor(&painter,rect,mouseMoveEventType);
            }
        }
        if(i == m_selectItemNum)
        {
            drawColor(&painter,rect,mousePressEventType);
        }
        painter.setPen(Qt::black);
        painter.drawText(rect,Qt::AlignCenter,m_texts.value(i));
        QRegion region(i * m_winWidth,0,m_winWidth,m_winHeight);
        m_Regions << region;
    }

    setMinimumWidth(m_winWidth * m_texts.size());
}

void QjtTabBar::drawColor(QPainter *painter,const QRect &rect,int type)
{
    if(type == mouseMoveEventType)
    {
        static QColor color0(247,255,255,0);
        static QColor color1(242,255,255,30);
        static QColor color2(245,255,255,50);
        QLinearGradient knobGradientTmp(rect.topLeft(),rect.bottomLeft());
        knobGradientTmp.setColorAt(0.0, color0);
        knobGradientTmp.setColorAt(0.8, color1);
        knobGradientTmp.setColorAt(1.0, color2);

        painter->setPen(Qt::NoPen);
        painter->setBrush(knobGradientTmp);
    }
    else if(type == mousePressEventType)
    {
        static QColor color0(247,255,255,0);
        static QColor color1(242,255,255,70);
        static QColor color2(245,255,255,100);
        QLinearGradient knobGradientTmp(rect.topLeft(),rect.bottomLeft());
//        knobGradientTmp.setColorAt(0.0, color0);
//        knobGradientTmp.setColorAt(0.3, color1);
//        knobGradientTmp.setColorAt(0.5, color2);
//        knobGradientTmp.setColorAt(0.9, color1);
//        knobGradientTmp.setColorAt(1.0, color0);

        knobGradientTmp.setColorAt(0.0, color0);
        knobGradientTmp.setColorAt(0.8, color1);
        knobGradientTmp.setColorAt(1.0, color2);


        painter->setPen(Qt::NoPen);
        painter->setBrush(knobGradientTmp);
    }

    painter->drawRect(rect);
}

