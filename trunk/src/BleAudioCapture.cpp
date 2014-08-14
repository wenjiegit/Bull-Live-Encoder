#include "BleAudioCapture.hpp"

#include <QString>

#include "RtAudio.h"
#include "BleUtil.hpp"
#include "BleLog.hpp"
#include "BleAVQueue.hpp"
#include "BleAudioEncoder_AAC.hpp"
#include "BleErrno.hpp"

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

        // if use aac, then should send aac sequence header.
        BleAudioEncoder_AAC *aac_encoder = dynamic_cast<BleAudioEncoder_AAC*>(m_audioEncoder);
        if (!m_hasSendHeader && aac_encoder) {
            BleAudioPacket *pkt = new BleAudioPacket(Audio_Type_AAC);
            pkt->data = aac_encoder->getHeader();

            BleAVQueue::instance()->enqueue(pkt);
            m_hasSendHeader = true;
        }

        int frameSize = m_audioEncoder->getFrameSize();

        m_mutex.lock();
        while (m_bytesCache.size() < frameSize) {
            m_waitCondtion.wait(&m_mutex);
        }

        while (m_bytesCache.size() >= frameSize) {
            QByteArray frame = m_bytesCache.mid(0, frameSize);
            m_bytesCache.remove(0, frameSize);

            QByteArray outputArray;
            if (!m_audioEncoder->encode(frame, outputArray)) {
                stopCapture();
                log_error("encode audio failed.");
                return;
            }

            if (outputArray.size() > 0) {
                BleAudioPacket *pkt = new BleAudioPacket(Audio_Type_AAC);
                pkt->data = outputArray;
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
        // TODO impl
    }
    BleAssert(m_audioEncoder);

    if (!m_audioEncoder->init(m_sampleRate, m_channels, m_bitrate)) {
        log_error("audio encoder error");
        return BLE_AUDIO_INIT_ERROR;
    }

    m_grabEngine = new RtAudio;

    unsigned int bufferFrames = 2048;
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

    BleAVQueue::instance()->timestampBuilder()->
            setAudioCaptureInternal(m_audioEncoder->getFrameDuration());

    start();

    return BLE_SUCESS;
}

int BleAudioCapture::stopCapture()
{
    if (m_grabEngine)
        m_grabEngine->closeStream();

    BleFree(m_grabEngine);

    return BLE_SUCESS;
}

void BleAudioCapture::onDataCaptured(char *data, int size)
{
    m_mutex.lock();
    m_bytesCache.append(data, size);

    if (m_bytesCache.size() >= m_audioEncoder->getFrameSize()) {
        m_mutex.unlock();

        m_waitCondtion.wakeOne();
    } else {
        m_mutex.unlock();
    }
}
