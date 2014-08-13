#ifndef BLEAUDIOENCODER_AAC_HPP
#define BLEAUDIOENCODER_AAC_HPP

#include <QByteArray>

#include "faac.h"

class BleAudioEncoder_AAC
{
public:
    BleAudioEncoder_AAC();

    virtual bool init(int samplerate, int channel, int bitrate);
    virtual void fini();
    virtual bool encode(const QByteArray &data, QByteArray &output);

public:
    //int sample_rate;
    int m_nFrameSize;
    int m_channels;
    //int bit_rate;

    unsigned long m_nSamplesInput;
    unsigned long m_nMaxbytesOutput;

    faacEncHandle   m_faacHandle;
    int*            m_pInBuf;
    QByteArray          m_outputBuffer;
};

#endif // BLEAUDIOENCODER_AAC_HPP
