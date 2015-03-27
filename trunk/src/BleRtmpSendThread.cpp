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
#include <QDir>

#include "BleRtmpMuxer.hpp"
#include "BleEncoderThread.hpp"
#include "BleLog.hpp"
#include "BleUtil.hpp"
#include "BleErrno.hpp"
#include "MOption.h"
#include "BleContext.hpp"
#include "mstream.hpp"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "BleAVQueue.hpp"

static char flv_header[] = {'F', 'L', 'V',
                            0x01, 0x05, 0x00, 0x00, 0x00, 0x09,
                            0x00, 0x00, 0x00, 0x00};

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

    m_rtmp_muxer = NULL;
    int ret = BLE_SUCESS;

    while (!m_stop) {
        QString url = MOption::instance()->option("address", "network").toString();
        if (url.isEmpty()) {
            log_error("no url was set, please input your rtmp url !");
            sleep(1);
            continue;
        }

        m_rtmp_muxer = new BleRtmpMuxer;
        BleAutoFree(BleRtmpMuxer, m_rtmp_muxer);

        m_rtmp_muxer->setRtmpUrl(url.toStdString());
        ret = m_rtmp_muxer->start();
        if (ret != TRUE) {
            ret = BLE_RTMPCONNECT_ERROR;
            int sleep_s = 1;
            log_error("connect to %s error, after %ds to retry", url.toStdString().c_str(), sleep_s);

            m_rtmp_muxer->stop();
            sleep(sleep_s);

            continue;
        }

        log_trace("connect to %s success.", url.toStdString().c_str());
        ret = service(*m_rtmp_muxer);
        m_rtmp_muxer->stop();

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

void BleRtmpSendThread::stop()
{
    m_rtmp_muxer->close_socket();
    BleThread::stop();
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

    if (on_record() != BLE_SUCESS) {
        log_trace("flv on_record error");
    }

    MStream metadata_body;
    if ((ret = sendMetadata(muxer, metadata_body)) != BLE_SUCESS) {
        return ret;
    }

    if ((ret = sendVideoSh(muxer)) != BLE_SUCESS) {
        return ret;
    }

    if ((ret = sendAudioSh(muxer)) != BLE_SUCESS) {
        return ret;
    }

    // write metadata
    record(metadata_body, 0, FLV_TAG_METADATA);

    // write video sh
    BleAVPacket *vs = appCtx->videoSh();
    if (vs) {
        record(vs->data, 0, FLV_TAG_VIDEO);
    }

    // write audio sh
    BleAVPacket *as = appCtx->audioSh();
    if (as) {
        record(as->data, 0, FLV_TAG_AUDIO);
    }

    while (!m_stop) {
        QQueue<BleAVPacket *> pkts = BleAVQueue::instance()->dequeue();
        if (pkts.isEmpty()) {
            msleep(50);
            continue;
        }

        BleAutoLocker(m_mutex);

        while (!pkts.empty() && !m_stop) {
            BleAVPacket *pkt = pkts.dequeue();
            BleAutoFree(BleAVPacket, pkt);

            MStream &data = pkt->data;

            if (pkt->pktType == Packet_Type_Video) {
                if (muxer.addH264(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    break;
                }
                record(data, pkt->dts, FLV_TAG_VIDEO);

                m_video_send_bytes += data.size();
                m_fps += 1;
            } else if (pkt->pktType == Packet_Type_Audio) {
                if (muxer.addAAC(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    break;
                }
                record(data, pkt->dts, FLV_TAG_AUDIO);

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
    on_un_record();

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

int BleRtmpSendThread::on_record()
{
    m_record_error = false;

    bool enableDvr = MOption::instance()->option("save_enabled", "Video_Save") == "true" ? true : false;
    if (!enableDvr) {
        m_record_error = true;
        return BLE_SUCESS;
    }

    // video save
    QString dir = MOption::instance()->option("save_path", "Video_Save").toString();
    if (dir.isEmpty()) {
        m_record_error = true;
        log_error("record dir is not set.");
        return -1;
    }

    QDir temp;
    if (!temp.mkpath(dir)) {
        log_error("mkdir error, dir=%s", dir.toStdString().c_str());
        m_record_error = true;
        return -1;
    }

    QString current_time = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    QString flv_name = dir + "/" + current_time + ".flv";
    m_record_file = new QFile(flv_name);
    if (!m_record_file->open(QIODevice::WriteOnly)) {
        log_error("open flv file error, file=%s", flv_name.toStdString().c_str());
        m_record_error = true;
        BleFree(m_record_file);
        return -1;
    }

    // write flv header
    if (m_record_file->write(flv_header, 13) < 0) {
        m_record_error = true;
        m_record_file->close();
        BleFree(m_record_file);
        return -1;
    }

    return BLE_SUCESS;
}

int BleRtmpSendThread::record(MStream &data, qint64 dts, int flv_tag_type)
{
    if (m_record_error) return BLE_SUCESS;

    MStream stream;

    // tag header
    mint8 type = flv_tag_type;

    int payload_size = data.length();

    stream.write1Bytes(type);
    stream.write3Bytes(payload_size);
    stream.write3Bytes(dts);
    stream.write1Bytes(dts >> 24 & 0xFF);
    stream.write3Bytes(0);
    stream.writeString(data);

    // pre tag size
    int preTagSize = payload_size + 11;
    stream.write4Bytes(preTagSize);

    // write flv tag
    if (m_record_file->write(stream.data(), stream.length()) < 0) {
        log_error("write flv tag error, disable record.");
        m_record_error = true;
        m_record_file->close();
        BleFree(m_record_file);
        return -1;
    }
    m_record_file->flush();

    return BLE_SUCESS;
}

int BleRtmpSendThread::on_un_record()
{
    if (!m_record_error && m_record_file && m_record_file->isOpen()) {
        log_trace("record finished. file save to %s"
                  , m_record_file->fileName().toStdString().c_str());

        m_record_file->close();
        BleFree(m_record_file);
    }

    m_record_error = false;

    return BLE_SUCESS;
}

int BleRtmpSendThread::sendMetadata(BleRtmpMuxer & muxer, MStream &body)
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

    if ((ret = muxer.setMetaData(*meta, body)) != TRUE) {
        return BLE_RTMPSEND_ERROR;
    }

    return BLE_SUCESS;
}
