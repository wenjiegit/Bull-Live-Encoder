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

#include "QjtCustomWidget.h"
#include "TitleWidget.h"
#include "NcFramelessHelper.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QtWin>

QjtCustomWidget::QjtCustomWidget(QWidget *parent) :
    QDialog(parent)
  , ThemedWidgetBase(this)
  , titleWidget(new TitleWidget(this))
  , vBoxLayout(new QVBoxLayout(this))
  , framelessHelper(new NcFramelessHelper)
{
    titleWidget->setHints(TitleWidget::CLOSE_HINT);
    titleWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(titleWidget);

    connect(titleWidget->button(TitleWidget::CLOSE_HINT), SIGNAL(clicked()),
            this, SLOT(onClose()));
    setLayout(vBoxLayout);

    framelessHelper->setWidgetMovable(true);
    framelessHelper->setWidgetResizable(true);
    framelessHelper->activateOn(this);

    m_titleHeight = titleWidget->height();
    m_linearHeight= 30;
    m_statusHeight= 0;

#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(this);
#endif
}

QjtCustomWidget::~QjtCustomWidget()
{

}

void QjtCustomWidget::setTitle(const QString &title)
{
    titleWidget->setTitle(title);
}

void QjtCustomWidget::setMovable(bool moveble)
{
    framelessHelper->setWidgetMovable(moveble);
}

void QjtCustomWidget::setResizable(bool resizable)
{
    framelessHelper->setWidgetResizable(resizable);
}

void QjtCustomWidget::setPaintHeight(int titleHeight, int linearHeight, int statusHeight)
{
    m_titleHeight = titleHeight;
    m_linearHeight = linearHeight;
    m_statusHeight = statusHeight;
}

void QjtCustomWidget::addWidget(QWidget *widget)
{
    widget->setParent(this);

    vBoxLayout->addWidget(widget);
}

void QjtCustomWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    ThemedWidgetBase::drawThemedStyle(p);
}

void QjtCustomWidget::onClose()
{
    close();
}
