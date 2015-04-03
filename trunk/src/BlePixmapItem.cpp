#include "BlePixmapItem.hpp"
#include "ui_BlePixmapItem.h"

#include <QPainter>

static QList<BlePixmapItem *> g_items;

BlePixmapItem::BlePixmapItem(QWidget *parent) :
    QPushButton(parent),
    ui(new Ui::BlePixmapItem)
  , m_enter(false)
  , m_selected(false)
{
    ui->setupUi(this);

    connect(this, SIGNAL(clicked()), this, SLOT(onPressed()));
    g_items << this;
}

BlePixmapItem::~BlePixmapItem()
{
    g_items.removeAll(this);
    delete ui;
}

void BlePixmapItem::setPixmap(const QPixmap &pix)
{
    m_icon = pix;
    update();
}

void BlePixmapItem::onPressed()
{
    foreach (BlePixmapItem *item, g_items) {
        if (item != this) {
            item->setSelected(false);
        } else  {
            item->setSelected(true);
        }
    }
}

void BlePixmapItem::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(QRect(1, 1, width()-2, height()-2), 1, 1);

    if (m_enter || m_selected) {
        QColor c = QColor("#daeafc");
        c.setAlpha(10);
        p.fillPath(path, QBrush(c));

        c = QColor(Qt::red);

        QPen pen(c);
        pen.setWidth(2);
        p.setPen(pen);
        p.drawPath(path);
    }

    QRect pixmapRect(0, 0, width()-10, height()-10);
    QRect topRect(rect());
    pixmapRect.moveCenter(topRect.center());

    if (!m_icon.isNull()) {
        p.drawPixmap(pixmapRect, m_icon);
    } else {
        QImage image(10, 10, QImage::Format_RGB888);
        image.fill(0x2c2c2c);
        p.drawPixmap(pixmapRect, QPixmap::fromImage(image));
    }
}

void BlePixmapItem::enterEvent(QEvent *)
{
    //m_enter = true;
    update();
}

void BlePixmapItem::leaveEvent(QEvent * event)
{
    //m_enter = false;
    update();
}

QSize BlePixmapItem::sizeHint() const
{
    return QSize(200, 120);
}

void BlePixmapItem::setSelected(bool selected)
{
    m_enter = selected;
    update();
}

//QSize StcPixmapItem::minimumSizeHint() const
//{
//    return QSize(120, 150);
//}
