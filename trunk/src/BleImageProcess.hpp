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

#ifndef BLEIMAGEPROCESS_H
#define BLEIMAGEPROCESS_H

#include <QWidget>
#include <QTimer>

#include "BleSourceAbstract.hpp"

namespace Ui {
class BleImageProcess;
}

class BleSourceAbstract;

class BleImageProcess : public QWidget
{
    Q_OBJECT

public:
    explicit BleImageProcess(QWidget *parent = 0);
    ~BleImageProcess();

    void setProcessThread(QThread *thread);
    void addCaptureSource(BleSourceAbstract *source, int x, int y, int w, int h);

    virtual void paintEvent(QPaintEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void focusOutEvent(QFocusEvent * e);

    void resizeEvent(QResizeEvent * event);

private slots:
    void onIncBtnClicked();
    void onDecBtnClicked();
    void onUpBtnClicked();
    void onDownBtnClicked();
    void onLeftBtnClicked();
    void onRightBtnClicked();
    void onRefreshTimeout();
    void onSettingChanged();

private:
    void updateSources();
    int getPairIndex(SourcePair *pair);

private:
    Ui::BleImageProcess *ui;

    QSize m_parentSize;
    QRect m_activeRect;
    SourcePair * m_activePair;
    bool m_startMove;
    bool m_startResize;
    bool m_resizeFromTopLeft;
    bool m_resizeFromBottomRight;
    QPoint m_lastMovePoint;
    QList<SourcePair> m_sources;

    // refresh timer to update pixmap
    QTimer m_refreshTimer;
    QThread * m_processThread;
};

#endif // BLEIMAGEPROCESS_H
