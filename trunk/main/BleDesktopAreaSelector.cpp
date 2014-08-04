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

#include "BleDesktopAreaSelector.hpp"
#include "ui_BleDesktopAreaSelector.h"

#include <QKeyEvent>
#include <QPen>
#include <QPainter>

#include <QDebug>

BleDesktopAreaSelector::BleDesktopAreaSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BleDesktopAreaSelector)
    , m_startSelect(false)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(0.9);
    setMouseTracking(true);
}

BleDesktopAreaSelector::~BleDesktopAreaSelector()
{
    delete ui;
}

void BleDesktopAreaSelector::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QPen pen(Qt::DotLine);
    pen.setWidth(3);
    pen.setColor(Qt::red);
    p.setPen(pen);

    p.drawRect(m_selectedRect);
}

void BleDesktopAreaSelector::keyPressEvent(QKeyEvent *event)
{
    // esc key pressed
    if (event->key() == Qt::Key_Escape) {
        this->hide();
        this->deleteLater();
    }
}

void BleDesktopAreaSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (m_startSelect) {
        m_selectedRect = QRect(m_startPoint, event->pos());
        update();
    }
}

void BleDesktopAreaSelector::mousePressEvent(QMouseEvent *e)
{
    m_startSelect = true;
    m_startPoint = e->pos();
}

void BleDesktopAreaSelector::mouseReleaseEvent(QMouseEvent *e)
{
    m_startSelect = false;
}

void BleDesktopAreaSelector::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_selectedRect.isValid()) {
        emit areaSelected(m_selectedRect);
    }

    hide();
}
