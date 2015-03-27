#include "BleTextSource.hpp"

#include <QPainter>

BleTextSource::BleTextSource(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    m_textImage = QImage(100, 30, QImage::Format_ARGB32);
    memset(m_textImage.bits(), 0, m_textImage.byteCount());
}

QString BleTextSource::getSourceName()
{
    return "BleTextSource";
}

QImage BleTextSource::getImage()
{
    return m_textImage;
}

void BleTextSource::stopCapture()
{

}

void BleTextSource::setText(const QString &text)
{
    m_text = text;

    onTimeout();
}

void BleTextSource::onTimeout()
{
    QPainter painter(&m_textImage);

    //painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    QPen pen = painter.pen();
    pen.setColor(Qt::blue);
    QFont font = painter.font();
    font.setBold(true);//加粗

    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(m_textImage.rect(),Qt::AlignCenter,"BLE 你好啊");

    //m_textImage = m_textImage.rgbSwapped();

    BleImage be;
    be.width = m_textImage.width();
    be.height = m_textImage.height();

    be.data = new char[m_textImage.byteCount()];
    memcpy(be.data, m_textImage.bits(), m_textImage.byteCount());

    be.dataSize = m_textImage.byteCount();
    be.format = BleImage_Format_RGBA;

    m_image = be;
}
