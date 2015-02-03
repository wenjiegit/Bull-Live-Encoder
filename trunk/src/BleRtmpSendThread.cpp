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

#include "BleRtmpSendThread.hpp"

#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>

#include "BleRtmpMuxer.hpp"
#include "BleEncoderThread.hpp"
#include "BleLog.hpp"
#include "BleUtil.hpp"
#include "BleErrno.hpp"
#include "MOption.h"
#include "BleContext.hpp"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "BleAVQueue.hpp"

BleRtmpSendThread::BleRtmpSendThread(QObject * parent)
    : BleThread(parent)
    , m_audio_send_bytes(0)
    , m_video_send_bytes(0)
    , m_fps(0)
    , m_data_send_bytes(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer.setInterval(1000 * 1);
    m_timer.start();
    m_elapsed_timer.start();
}

BleRtmpSendThread::~BleRtmpSendThread()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
}

void BleRtmpSendThread::run()
{
#ifdef Q_OS_WIN
    if (wsaStart()) {
        return;
    }
#endif

    int ret = BLE_SUCESS;
    while (!m_stop) {
        QString url = MOption::instance()->option("address", "network").toString();
        if (url.isEmpty()) {
            log_error("no url was set, please input your rtmp url !");
            sleep(5);
            continue;
        }

        BleRtmpMuxer *muxer = new BleRtmpMuxer;
        BleAutoFree(BleRtmpMuxer, muxer);

        muxer->setRtmpUrl(url.toStdString());
        ret = muxer->start();
        if (ret != TRUE) {
            ret = BLE_RTMPCONNECT_ERROR;
            log_error("connect to %s error, after 3s to retry", url.toStdString().c_str());

            muxer->stop();
            sleep(3);

            continue;
        }

        log_trace("connect to %s success.", url.toStdString().c_str());
        ret = service(*muxer);
        muxer->stop();

        if (ret != BLE_SUCESS) {
            log_error("rtmp send error, ret = %d", ret);
            continue;
        }
    }

#ifdef Q_OS_WIN
    WSACleanup();
#endif

    log_trace("Ble RtmpSendThread exit normally.");
}

#ifdef Q_OS_WIN
int BleRtmpSendThread::wsaStart()
{
    WORD versionRequested;
    WSADATA wsaData;
    versionRequested = MAKEWORD(1, 1);

    return WSAStartup(versionRequested, &wsaData);
}
#endif

int BleRtmpSendThread::service(BleRtmpMuxer & muxer)
{
    int ret = BLE_SUCESS;

    if ((ret = sendMetadata(muxer)) != BLE_SUCESS) {
        return ret;
    }

    if ((ret = sendVideoSh(muxer)) != BLE_SUCESS) {
        return ret;
    }

    if ((ret = sendAudioSh(muxer)) != BLE_SUCESS) {
        return ret;
    }

    while (!m_stop) {
        QQueue<BleAVPacket *> pkts = BleAVQueue::instance()->dequeue();
        if (pkts.isEmpty()) {
            msleep(50);
            continue;
        }

        BleAutoLocker(m_mutex);

        while (!pkts.empty()) {
            BleAVPacket *pkt = pkts.dequeue();
            BleAutoFree(BleAVPacket, pkt);

            MStream &data = pkt->data;

            if (pkt->pktType == Packet_Type_Video) {
                if (muxer.addH264(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    break;
                }
                // log_trace("------------------>  V  %lld", pkt->dts);

                m_video_send_bytes += data.size();
                m_fps += 1;
            } else if (pkt->pktType == Packet_Type_Audio) {
                if (muxer.addAAC(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    break;
                }
                //log_trace("------------------>  A  %lld", pkt->dts);

                m_audio_send_bytes += data.size();
            }

            m_data_send_bytes += data.size();
        }

        // if send failed, then pkts may has some pkt
        // we should delete it.
        for (int i = 0; i < pkts.size(); ++i) {
            BleAVPacket *pkt = pkts.at(i);
            BleFree(pkt);
        }
    }

    return ret;
}

int BleRtmpSendThread::sendVideoSh(BleRtmpMuxer &muxer)
{
    int ret = BLE_SUCESS;

    // if H264 send video sh
    BleAVPacket *pkt = appCtx->videoSh();
    if (pkt) {
        MStream &data = pkt->data;
        if (muxer.addH264(data, pkt->dts) != TRUE ) {
            ret = BLE_RTMPSEND_ERROR;
            return ret;
        }

        log_trace("H264 send video sh success");
    }

    return ret;
}

int BleRtmpSendThread::sendAudioSh(BleRtmpMuxer &muxer)
{
    int ret = BLE_SUCESS;

    // if AAC, send audio sh
    BleAVPacket *pkt = appCtx->audioSh();
    if (pkt) {
        MStream &data = pkt->data;
        if (muxer.addAAC(data, pkt->dts) != TRUE ) {
            ret = BLE_RTMPSEND_ERROR;
            return ret;
        }

        log_trace("AAC send audio sh success");
    }

    return ret;
}

void BleRtmpSendThread::onTimeout()
{
    if (!m_mutex.tryLock()) return;

    int elapsed_ms = m_elapsed_timer.elapsed();

    float audioKbps = m_audio_send_bytes * 1000 / elapsed_ms;
    float videoKbps = m_video_send_bytes * 1000 / elapsed_ms;
    float fps = (float)m_fps * 1000.00 / (float)elapsed_ms;

    m_audio_send_bytes = 0;
    m_video_send_bytes = 0;
    m_fps = 0;

    m_elapsed_timer.restart();
    m_mutex.unlock();

    kbps bs = {audioKbps, videoKbps, fps};
    m_kbps.append(bs);

    // average value
    if (m_kbps.size() > 10) m_kbps.removeFirst();

    float audioKbpsAll = 0;
    float videoKbpsAll = 0;
    float fpsALl = 0;
    for (int i = 0; i < m_kbps.size(); ++i) {
        const kbps &bs = m_kbps.at(i);
        audioKbpsAll += bs.audio_avg;
        videoKbpsAll += bs.video_avg;
        fpsALl += bs.fps;
    }

    audioKbps = audioKbpsAll / m_kbps.size();
    videoKbps = videoKbpsAll / m_kbps.size();
    fps = fpsALl / m_kbps.size();

    emit status(audioKbps, videoKbps, fps, m_data_send_bytes);
}

int BleRtmpSendThread::sendMetadata(BleRtmpMuxer & muxer)
{
    int ret = BLE_SUCESS;

    MOption *option = MOption::instance();

    QSize wh = option->option("res", "encoder").toSize();

    int audiocodecid = CODECID_AAC;
    int audiodatarate = option->option("bitrate", "audio").toInt();
    int audiodelay = 0;
    int audiosamplerate = option->option("sample_rate", "audio").toInt();
    int audiosamplesize = 1024;
    bool canSeekToEnd = false;

    int duration = 0;
    int filesize = 0;
    int framerate = option->option("fps", "encoder").toInt();
    int height = wh.height();
    bool stereo = option->option("channels", "audio").toString() == "Stereo" ? true : false;
    int videocodecid = CODECID_H264;
    int videodatarate = option->option("bitrate", "encoder").toInt();
    int width = wh.width();;

    FlvMetaData *meta = new FlvMetaData;
    BleAutoFree(FlvMetaData, meta);
    meta->audiocodecid = audiocodecid;
    meta->audiodatarate = audiodatarate;
    meta->audiodelay = audiodelay;
    meta->audiosamplerate = audiosamplerate;
    meta->audiosamplesize = audiosamplesize;
    meta->canSeekToEnd = canSeekToEnd;
    meta->duration = duration;
    meta->filesize = filesize;
    meta->framerate = framerate;
    meta->height = height;
    meta->stereo = stereo;
    meta->videocodecid = videocodecid;
    meta->videodatarate = videodatarate;
    meta->width = width;
    meta->creationdate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();

    if ((ret = muxer.setMetaData(*meta)) != TRUE) {
        return BLE_RTMPSEND_ERROR;
    }

    return BLE_SUCESS;
}
