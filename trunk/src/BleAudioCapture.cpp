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

#include "BleAudioCapture.hpp"

#include <QString>
#include <QElapsedTimer>

#include "RtAudio.h"
#include "BleUtil.hpp"
#include "BleLog.hpp"
#include "BleAVQueue.hpp"
#include "BleAudioEncoder_AAC.hpp"
#include "BleAudioEncoder_MP3.hpp"
#include "BleErrno.hpp"
#include "BleContext.hpp"
#include "BleAVContext.hpp"
#include "BleVLCPlayer.hpp"

#include "MOption.h"

int handleAudioData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data)
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(status)

    BleAudioCapture *grabber = static_cast<BleAudioCapture *>(data);

    int bytesReady = nBufferFrames * 16 / 8 * grabber->m_channels;

    grabber->onDataCaptured((char*)inputBuffer, bytesReady);

    return BLE_SUCESS;
}

BleAudioCapture::BleAudioCapture(QObject *parent)
    : BleThread(parent)
    , m_grabEngine(NULL)
    , m_audioEncoder(NULL)
    , m_hasSendHeader(false)
    , m_bitDepth(RTAUDIO_SINT16)
    , m_sampleRate(44100)
    , m_channels(2)
    , m_bitrate(96000)
{

}

BleAudioCapture::~BleAudioCapture()
{

}

void BleAudioCapture::run()
{
    m_stop = false;

    while (!m_stop) {
        int frameSize = m_audioEncoder->getFrameSize();

        m_mutex.lock();

        if (m_bytesCache.size() < frameSize) {
            m_mutex.unlock();
            msleep(20);
            continue;
        }

        while (m_bytesCache.size() >= frameSize) {
            QByteArray frame = m_bytesCache.mid(0, frameSize);
            m_bytesCache.remove(0, frameSize);

            // mix all audio
            QList<BleVLCPlayer *> players = BleAVContext::instance()->getPlayers();
            for (int i = 0; i < players.size(); ++i) {
                BleVLCPlayer *player = players.at(i);
                QByteArray playerAudioFrame;

                if (player->getAudioSamples(frameSize, playerAudioFrame) != BLE_SUCESS) {
                    continue;
                }

                audioMix(frame, playerAudioFrame);
            }

            QByteArray outputArray;
            if (!m_audioEncoder->encode(frame, outputArray)) {
                stopCapture();
                log_error("encode audio failed.");
                return;
            }

            // the output should > 0
            if (!outputArray.isEmpty()) {
                BleAudioPacket *pkt = new BleAudioPacket(Audio_Type_AAC);
                pkt->data.writeString(MString(outputArray.data(), outputArray.size()));

                pkt->dts = BleAVQueue::instance()->timestampBuilder()->addAudioFrame();
                pkt->has_encoded = true;
                BleAVQueue::instance()->enqueue(pkt);
            }
        }

        m_mutex.unlock();
    }

    log_trace("BleAudioCapture Thread exit normally");
}

QHash<int, QString> BleAudioCapture::availableDevices()
{
    RtAudio rtAudio;
    int deviceCount = rtAudio.getDeviceCount();
    RtAudio::DeviceInfo info;
    QHash<int, QString> devices;

    for (int i = 0; i < deviceCount; ++i) {
        info = rtAudio.getDeviceInfo(i);

        if (info.inputChannels > 0) {
            devices.insert(i, QString::fromStdString(info.name));
        }
    }

    return devices;
}

int BleAudioCapture::startCapture(int bitrate, int sampleRate, int channels, int deviceID)
{
    m_bitrate = bitrate;
    m_sampleRate = sampleRate;
    m_channels = channels;

    QString audioFormat = MOption::instance()->option("format", "audio").toString();
    if (audioFormat == "AAC") {
        m_audioEncoder = new BleAudioEncoder_AAC;
    } else if (audioFormat == "MP3") {
        m_audioEncoder = new BleAudioEncoder_MP3;
    }
    BleAssert(m_audioEncoder);

    if (!m_audioEncoder->init(m_sampleRate, m_channels, m_bitrate)) {
        log_error("audio encoder error");
        return BLE_AUDIO_INIT_ERROR;
    }

    if (m_audioEncoder->encoderType() == BleAudioEncoderAbstract::AAC) {
        // update audio sh
        BleAudioPacket *pkt = new BleAudioPacket(Audio_Type_AAC);
        QByteArray arr = dynamic_cast<BleAudioEncoder_AAC*>(m_audioEncoder)->getHeader();
        pkt->data.writeString(arr.data(), arr.size());
        pkt->has_encoded = true;
        pkt->dts = 0;

        appCtx->setAudioSh(pkt);
    }

    m_grabEngine = new RtAudio;

    unsigned int bufferFrames = m_audioEncoder->getFrameSize() / 2 / m_channels;
    RtAudio::StreamParameters params;

    if (deviceID == -1) {
        deviceID = m_grabEngine->getDefaultInputDevice();
    }
    params.deviceId = deviceID;
    params.nChannels = m_channels;
    params.firstChannel = 0;

    try {
        m_grabEngine->openStream(NULL, &params, m_bitDepth, m_sampleRate, &bufferFrames, &handleAudioData, this);
        m_grabEngine->startStream();
    } catch (RtError& e) {
        e.printMessage();
        BleFree(m_grabEngine);

        return BLE_AUDIO_DEVICE_OPEN_ERROR;
    }

    float frameDuration = m_audioEncoder->getFrameDuration();
    BleAVQueue::instance()->timestampBuilder()->setAudioCaptureInternal(frameDuration);

    start();

    return BLE_SUCESS;
}

int BleAudioCapture::stopCapture()
{
    if (m_grabEngine)
        m_grabEngine->closeStream();

    BleFree(m_grabEngine);

    if (m_audioEncoder) {
        m_audioEncoder->fini();
    }
    BleFree(m_audioEncoder);

    return BLE_SUCESS;
}

void BleAudioCapture::onDataCaptured(char *data, int size)
{
    BleAutoLocker(m_mutex);
    m_bytesCache.append(data, size);
}

void BleAudioCapture::audioMix(QByteArray &a1, QByteArray &a2)
{
    if (a1.size() != a2.size()) return;

    // TODO process PCM overflow
    // static short pcmMax = 0x8FFF;

    short *pcm_1 = ( short *)a1.data();
    short *pcm_2 = ( short *)a2.data();

    int size = a1.size() / sizeof(short);
    for (int i = 0; i < size; ++i) {
        pcm_1[i] = (float)pcm_1[i] * 0.5 + (float)pcm_2[i] * 0.5;
    }
}
