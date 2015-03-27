#include "BleResizeableWidget.hpp"

#include <QPainter>

BleResizeableWidget::BleResizeableWidget(QWidget *parent) :
    QWidget(parent)
{
    setEditable(true);
}

void BleResizeableWidget::setEditable(bool enable)
{
    m_editable = enable;
}

void BleResizeableWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    if (m_editable) {
        QRect r(0, 0, width()-1, height()-1);
        p.drawRect(r);
    }
}
