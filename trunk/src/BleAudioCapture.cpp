#include "BleAudioCapture.hpp"

#include <QString>

#include "RtAudio.h"
#include "BleUtil.hpp"
#include "BleLog.hpp"

int handleAudioData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data)
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(status)

    BleAudioCapture *grabber = static_cast<BleAudioCapture *>(data);

    int bytesReady = nBufferFrames * 2 * 2;
    QByteArray array;
    array.resize(bytesReady);
    memcpy(array.data(), inputBuffer, bytesReady);

    grabber->onDataCaptured(array);

    return 0;
}

#include "BleAudioEncoder_AAC.hpp"

BleAudioEncoder_AAC *aac;

BleAudioCapture::BleAudioCapture()
    : m_grabEngine(NULL)
{
    aac = new BleAudioEncoder_AAC;
    aac->init(44100, 2, 96);
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

int BleAudioCapture::startCapture(int deviceID)
{
    m_grabEngine = new RtAudio;

    unsigned int bufferFrames = 2048;
    RtAudio::StreamParameters params;

    if (deviceID == -1) {
        deviceID = m_grabEngine->getDefaultInputDevice();
    }
    params.deviceId = deviceID;
    params.nChannels = 2;
    params.firstChannel = 0;

    try {
        m_grabEngine->openStream(NULL, &params, RTAUDIO_SINT16, 44100, &bufferFrames, &handleAudioData, this);
        m_grabEngine->startStream();
    } catch (RtError& e) {
        e.printMessage();
        return -1;
    }

    return 0;
}

int BleAudioCapture::stopCapture()
{
    m_grabEngine->closeStream();

    BleFree(m_grabEngine);

    return 0;
}

void BleAudioCapture::onDataCaptured(const QByteArray &data)
{
    log_trace("audio data available, size = %d", data.size());
//    static unsigned char* dst = new unsigned char[2048];
//    int dstSize = 0;
//    aac->encode(data.data(), data.size(), dst, dstSize);
}
