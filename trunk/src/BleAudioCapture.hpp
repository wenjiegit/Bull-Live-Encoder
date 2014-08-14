#ifndef BLEAUDIOCAPTURE_HPP
#define BLEAUDIOCAPTURE_HPP

#include <QHash>
#include <QMutex>
#include <QWaitCondition>

#include "BleThread.hpp"
#include "RtAudio.h"

class BleAudioEncoderAbstract;

class BleAudioCapture : public BleThread
{
    Q_OBJECT
public:
    BleAudioCapture(QObject * parent = 0);
    ~BleAudioCapture();

    virtual void run();

    static QHash<int, QString> availableDevices();

private:
    void onDataCaptured(char *data, int size);

private:
    RtAudio *m_grabEngine;
    BleAudioEncoderAbstract *m_audioEncoder;
    QMutex m_mutex;
    QWaitCondition m_waitCondtion;

    QByteArray m_bytesCache;
    bool m_hasSendHeader;
    int m_bitDepth;
    int m_sampleRate;
    int m_channels;
    int m_bitrate;

public:
    friend int handleAudioData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                          double streamTime, RtAudioStreamStatus status, void *data);

    int startCapture(int bitrate = 96000, int sampleRate = 44100, int channels = 2, int deviceID = -1);
    int stopCapture();

};

#endif // BLEAUDIOCAPTURE_HPP
