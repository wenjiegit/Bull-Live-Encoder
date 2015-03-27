#include "BleNetStreamSource.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QDir>

#include "BleUtil.hpp"
#include "BleVLCPlayer.hpp"
#include "BleErrno.hpp"

BleNetStreamSource::BleNetStreamSource(QObject *parent)
    : BleThread(parent)
{
    m_vlcPlayer = new BleVLCPlayer;
}

QString BleNetStreamSource::getSourceName()
{
    return "BleNetStreamSource";
}

QImage BleNetStreamSource::getImage()
{
    return m_vlcPlayer->getImage();
}

void BleNetStreamSource::stopCapture()
{
    this->stop();
    this->wait();
    this->deleteLater();
}

void BleNetStreamSource::run()
{
    if (m_vlcPlayer->start() != BLE_SUCESS) {
        log_error("start vlc player error");
        return;
    }

    while (!m_stop) {
        msleep(100);
    }

    m_vlcPlayer->stop();

    // for test rtsp
    // rtsp://218.204.223.237:554/live/1/0547424F573B085C/gsfp90ef4k0a6iap.sdp

    log_trace("BleFileSource Thread exit normally.");
}

void BleNetStreamSource::setMedia(const QString &fileName)
{
    m_vlcPlayer->setMRL(fileName);
    m_vlcPlayer->setMediaType(BleVLCPlayer::MEDIA_TYPE_NETSTREAM);
}
