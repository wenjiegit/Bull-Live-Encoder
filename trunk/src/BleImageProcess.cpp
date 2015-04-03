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

#include "BleImageProcess.hpp"
#include "ui_BleImageProcess.h"

#include <QPainter>
#include <QPen>
#include <QEvent>
#include <QResizeEvent>

#include "BleLog.hpp"
#include "MOption.h"
#include "BleImageProcessThread.hpp"
#include "BleUtil.hpp"

BleImageProcess::BleImageProcess(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BleImageProcess)
    , m_activePair(NULL)
    , m_startMove(false)
    , m_startResize(false)
    , m_resizeFromTopLeft(false)
    , m_resizeFromBottomRight(false)
    , m_processThread(NULL)
{
    ui->setupUi(this);

    connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimeout()));
    m_refreshTimer.start(60);

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    onSettingChanged();
}

BleImageProcess::~BleImageProcess()
{
    for (int i = 0; i < m_sources.size(); ++i) {
        SourcePair & pair = m_sources[i];
        pair.source->stopCapture();
    }

    delete ui;
}

void BleImageProcess::setProcessThread(QThread *thread)
{
    m_processThread = thread;

    updateSources();
}

void BleImageProcess::addCaptureSource(BleSourceAbstract *source, int x, int y, int w, int h)
{
    QRect rt = QRect(x, y, w, h);
    SourcePair pair = {source, rt};
    m_sources.append(pair);
    m_activePair = &m_sources.front();

    updateSources();
    update();
}

void BleImageProcess::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // back ground
    p.fillRect(rect(), QBrush(QColor(20, 20, 20)));

    // element draw
    for (int i = 0; i < m_sources.size(); ++i) {
        const SourcePair & pair = m_sources.at(i);

        QImage sourceImage = pair.source->getImage();
        p.drawPixmap(pair.rect, QPixmap::fromImage(sourceImage));
    }

    if (m_activePair && m_activePair->rect.isValid()) {
        QPen pen(Qt::SolidLine);

        pen.setColor(Qt::white);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
        p.setPen(pen);
        p.drawRect(m_activePair->rect);

        QRect topLeftRect(m_activePair->rect.x(), m_activePair->rect.y(), 8, 8);
        p.fillRect(topLeftRect, QBrush(Qt::red));

        QRect bottomRightRect(m_activePair->rect.bottomRight().x(), m_activePair->rect.bottomRight().y(), -8, -8);
        p.fillRect(bottomRightRect, QBrush(Qt::red));
    }
}

void BleImageProcess::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_activePair) return;

    QRect topLeftRect(m_activePair->rect.x(), m_activePair->rect.y(), 8, 8);
    QRect bottomRightRect(m_activePair->rect.bottomRight().x() - 8, m_activePair->rect.bottomRight().y() - 8, 8, 8);

    if (topLeftRect.contains(event->pos())) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (bottomRightRect.contains(event->pos())) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (m_activePair->rect.contains(event->pos())) {
        setCursor(Qt::SizeAllCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    if (m_startResize) {
        if (m_resizeFromTopLeft) {
            m_activePair->rect.setTopLeft(event->pos());
        }

        if (m_resizeFromBottomRight){
            m_activePair->rect.setBottomRight(event->pos());
        }
    }

    if (m_startMove) {
        QPoint diff = QPoint(event->pos().x() - m_lastMovePoint.x(), event->pos().y() - m_lastMovePoint.y());

        int w = m_activePair->rect.width();
        int h = m_activePair->rect.height();
        m_activePair->rect.setTopLeft(QPoint(m_activePair->rect.x() + diff.x(), m_activePair->rect.y() + diff.y()));
        m_activePair->rect.setWidth(w);
        m_activePair->rect.setHeight(h);
    }

    if (m_startResize || m_startMove) {
        updateSources();
    }

    update();
    m_lastMovePoint = event->pos();
}

void BleImageProcess::mousePressEvent(QMouseEvent *e)
{
    int findIndex = -1;
    for (int i = m_sources.size() - 1; i >= 0; --i) {
        SourcePair & pair = m_sources[i];
         if (pair.rect.contains(e->pos())) {
             findIndex = i;
             break;
         }
    }

    if (findIndex != -1) {
        m_activePair = &m_sources[findIndex];
    } else {
        m_activePair = NULL;
    }

    if (!m_activePair) return;

    QRect topLeftRect(m_activePair->rect.x(), m_activePair->rect.y(), 8, 8);
    QRect bottomRightRect(m_activePair->rect.bottomRight().x(), m_activePair->rect.bottomRight().y(), -8, -8);

    if (m_activePair->rect.contains(e->pos())) {
        if (topLeftRect.contains(e->pos()))
        {
            m_startResize = true;
            m_resizeFromTopLeft = true;
        } else if (bottomRightRect.contains(e->pos())) {
            m_startResize = true;
            m_resizeFromBottomRight = true;
        } else {
            m_startMove = true;
        }

    } else {
        m_startMove = false;
    }

    m_lastMovePoint = e->pos();
}

void BleImageProcess::mouseReleaseEvent(QMouseEvent */*e*/)
{
    m_startMove = false;
    m_startResize = false;
    m_resizeFromTopLeft = false;
    m_resizeFromBottomRight = false;
}

void BleImageProcess::mouseDoubleClickEvent(QMouseEvent */*e*/)
{
    if (!m_activePair) return;

    QImage image = m_activePair->source->getImage();
    if (!image.isNull()) {
        QRect r(m_activePair->rect.topLeft(), image.size());
        m_activePair->rect = r;
    }

    updateSources();
    update();
}

void BleImageProcess::keyPressEvent(QKeyEvent *e)
{
    if (!m_activePair) return;

    int key = e->key();
    if (key == Qt::Key_Delete) {
        m_activePair->source->stopCapture();

        if (m_sources.contains(*m_activePair)) {
            m_sources.removeAll(*m_activePair);
            m_activePair = NULL;
        }

        updateSources();
        update();
    } else if (key == Qt::Key_Up) {
        onUpBtnClicked();
    } else if (key == Qt::Key_Down) {
        onDownBtnClicked();
    } else if (key == Qt::Key_Left) {
        onLeftBtnClicked();
    } else if (key == Qt::Key_Right) {
        onRightBtnClicked();
    } else if (key == Qt::Key_PageUp) {
        onIncBtnClicked();
    } else if (key == Qt::Key_PageDown) {
        onDecBtnClicked();
    } else {
        QWidget::keyPressEvent(e);
    }
}

void BleImageProcess::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e);

    m_activePair = NULL;
}
#include <QDebug>
void BleImageProcess::resizeEvent(QResizeEvent *event)
{
    QSize si = event->size();
    QSize oldSi = event->oldSize();

//QList<SourcePair> m_sources;
    float xV = (float)si.width() / oldSi.width();
    float yV = (float)si.height() / oldSi.height();
    for (int i = 0; i < m_sources.size(); ++i) {
        SourcePair &pair = m_sources[i];
        QRect oldRect = pair.rect;
        QRect newRect(xV * oldRect.x(), yV * oldRect.y(), xV * oldRect.width(), yV * oldRect.height());
        pair.rect = newRect;
    }

    qDebug() << event->size() << event->oldSize() << xV <<yV;


    update();
}

void BleImageProcess::onIncBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int w =  r.width();
        int h = r.height();
        r.setWidth(w + 1);
        r.setHeight(h + 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onDecBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int w =  r.width();
        int h = r.height();
        r.setWidth(w - 1);
        r.setHeight(h - 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onUpBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int y = r.y();
        r.moveTop(y - 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onDownBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int y = r.y();
        r.moveBottom(y + r.height() + 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onLeftBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int x =  r.x();
        r.moveLeft(x - 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onRightBtnClicked()
{
    if (!m_activePair) return;

    QRect &r = m_activePair->rect;
    if (r.isValid()) {
        int x =  r.x();
        r.moveRight(x + r.width() + 1);
    }

    update();
    updateSources();
}

void BleImageProcess::onRefreshTimeout()
{
    if (!m_sources.empty())
        update();
}

void BleImageProcess::onSettingChanged()
{
//    QSize si = MOption::instance()->option("res", "encoder").toSize();
//    setFixedSize(si);
}

void BleImageProcess::updateSources()
{
    QList<SourcePair> sources = m_sources;

    QSize si = MOption::instance()->option("res", "encoder").toSize();
    QSize oldSi = size();

    float xV = (float)si.width() / (float)oldSi.width();
    float yV = (float)si.height() / (float)oldSi.height();

    for (int i = 0; i < sources.size(); ++i) {
        SourcePair &pair = sources[i];
        QRect oldRect = pair.rect;
        QRect newRect(xV * (float)oldRect.x(), yV * (float)oldRect.y(), xV * (float)oldRect.width(), yV * (float)oldRect.height());
        pair.rect = newRect;
    }

    BleImageProcessThread *ipt = dynamic_cast<BleImageProcessThread*>(m_processThread);
    if (ipt) {
        ipt->updateSources(sources);
    }
}

int BleImageProcess::getPairIndex(SourcePair *pair)
{
    if (!pair) return -1;
    QList<SourcePair> m_sources;

    for (int i = 0; i < m_sources.size(); ++i) {
        SourcePair *p = &m_sources[i];

        if (pair == p) {
            return i;
        }
    }

    return -1;
}
