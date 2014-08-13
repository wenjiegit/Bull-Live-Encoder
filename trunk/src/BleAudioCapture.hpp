#ifndef BLEAUDIOCAPTURE_HPP
#define BLEAUDIOCAPTURE_HPP

#include <QHash>
#include "RtAudio.h"

class BleAudioCapture
{
public:
    BleAudioCapture();

    static QHash<int, QString> availableDevices();

private:
    void onDataCaptured(const QByteArray &data);

private:
    RtAudio *m_grabEngine;

public:
    friend int handleAudioData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                          double streamTime, RtAudioStreamStatus status, void *data);

    int startCapture(int deviceID = -1);
    int stopCapture();
};

#endif // BLEAUDIOCAPTURE_HPP
