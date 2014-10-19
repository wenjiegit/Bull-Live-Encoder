#include "BleTextSource.hpp"

#include <QPainter>

BleTextSource::BleTextSource(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    m_textImage = QImage(100, 30, QImage::Format_RGB888);
    m_textImage.fill(Qt::black);
}

BleImage BleTextSource::getImage()
{
    return m_image.clone();
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

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    QFont font = painter.font();
    font.setBold(true);//加粗

    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(m_textImage.rect(),Qt::AlignCenter,"BLE 你好啊");

    BleImage be;
    be.width = m_textImage.width();
    be.height = m_textImage.height();

    be.data = new char[m_textImage.byteCount()];
    memcpy(be.data, m_textImage.bits(), m_textImage.byteCount());

    be.dataSize = m_textImage.byteCount();
    be.format = BleImage_Format_BGR24;

    m_image = be;
}
