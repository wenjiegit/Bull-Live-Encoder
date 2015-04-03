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

#include "BleNetStreamSource.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QDir>

#include "BleUtil.hpp"
#include "BleVLCPlayer.hpp"
#include "BleErrno.hpp"
#include "MOption.h"

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
    int audioSampleRate = MOption::instance()->option("sample_rate", "audio").toInt();

    int audioChannels = 2;
    QString audioChannelsStr = MOption::instance()->option("channels", "audio").toString();
    if (audioChannelsStr == "Mono") {
        audioChannels = 1;
    } else if (audioChannelsStr == "Stereo") {
        audioChannels = 2;
    }

    m_vlcPlayer->setAudioInfo(audioSampleRate, audioChannels);

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
