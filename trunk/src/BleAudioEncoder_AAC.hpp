#ifndef BLEAUDIOENCODER_AAC_HPP
#define BLEAUDIOENCODER_AAC_HPP

#include <QByteArray>

#include "faac.h"
#include "BleAudioEncoderAbstract.hpp"

class BleAudioEncoder_AAC : public BleAudioEncoderAbstract
{
public:
    BleAudioEncoder_AAC();

    virtual bool init(int samplerate, int channel, int bitrate);
    virtual void fini();
    virtual bool encode(const QByteArray &data, QByteArray &output);\
    virtual int getFrameSize();
    virtual int getFrameDuration();

    QByteArray getHeader();

public:
    //int sample_rate;
    int m_nFrameSize;
    int m_channels;

    unsigned long m_samplesInputSize;
    unsigned long m_maxOutputSize;

    faacEncHandle   m_faacHandle;
    int*            m_pInBuf;
    unsigned char *m_outputBuffer;
    QByteArray m_header;
};

#endif // BLEAUDIOENCODER_AAC_HPP
