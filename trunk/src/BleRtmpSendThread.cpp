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
{
}

void BleRtmpSendThread::setEncodeThread(QThread * thread)
{
    m_encodeThread = thread;
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

        BleRtmpMuxer muxer;
        muxer.setRtmpUrl(url.toStdString());
        ret = muxer.start();
        if (ret != TRUE) {
            ret = BLE_RTMPCONNECT_ERROR;
            log_error("connect to %s error, after 3s to retry", url.toStdString().c_str());

            muxer.stop();
            sleep(3);

            continue;
        }
        log_trace("connect to %s success.", url.toStdString().c_str());

        ret = service(muxer);
        muxer.stop();

        if (ret != BLE_SUCESS) {
            log_error("rtmp send error, ret = %d", ret);
            continue;
        }
    }

    log_trace("Ble RtmpSendThread exit normally.");
}

#ifdef Q_OS_WIN
int BleRtmpSendThread::wsaStart()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested =MAKEWORD( 1, 1 );
    int err = WSAStartup( wVersionRequested, &wsaData );

    return err;
}

int BleRtmpSendThread::service(BleRtmpMuxer & muxer)
{
    int ret = BLE_SUCESS;

    BleEncoderThread *encodeThread = dynamic_cast<BleEncoderThread*>(m_encodeThread);
    BleAssert(encodeThread);

    // if H264 + AAC
    // send sequence header
    // send video sh
    {
        BleAVPacket *pkt = appCtx->videoSH;
        MStream &data = pkt->data;
        if (muxer.addH264(data, pkt->dts) != TRUE ) {
            ret = BLE_RTMPSEND_ERROR;
            return ret;
        }
    }

    // send audio sh
    {
        BleAVPacket *pkt = appCtx->audioSH;
        MStream &data = pkt->data;
        if (muxer.addAAC(data, pkt->dts) != TRUE ) {
            ret = BLE_RTMPSEND_ERROR;
            return ret;
        }
    }

    // send video sei
    {
        BleAVPacket *pkt = appCtx->seiPkt;
        MStream &data = pkt->data;
        if (muxer.addH264(data, pkt->dts) != TRUE ) {
            ret = BLE_RTMPSEND_ERROR;
            return ret;
        }
    }

    while (!m_stop) {
        // get from queue
        QElapsedTimer timer;
        timer.start();
        QQueue<BleAVPacket *> pkts = BleAVQueue::instance()->dequeue();
        if (pkts.isEmpty()) {
            msleep(50);
            continue;
        }
        log_info("get pkts count = %d", pkts.size());

        while (!pkts.empty()) {
            BleAVPacket *pkt = pkts.dequeue();
            MStream &data = pkt->data;

            if (pkt->pktType == Packet_Type_Video) {
                if (muxer.addH264(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    return ret;
                }
            } else if (pkt->pktType == Packet_Type_Audio) {
                if (muxer.addAAC(data, pkt->dts) != TRUE ) {
                    ret = BLE_RTMPSEND_ERROR;
                    return ret;
                }
            }
            log_info("send pkt pts = %d  %d", pkt->pts, timer.elapsed());

            BleFree(pkt);
        }
    }

    return ret;
}
#endif
