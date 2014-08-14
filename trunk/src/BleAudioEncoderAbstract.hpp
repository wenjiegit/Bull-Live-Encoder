#ifndef BLEAUDIOENCODERABSTRACT_HPP
#define BLEAUDIOENCODERABSTRACT_HPP

#include <QByteArray>

class BleAudioEncoderAbstract
{
public:
    BleAudioEncoderAbstract();

    virtual bool init(int samplerate, int channel, int bitrate) = 0;
    virtual void fini() = 0;
    virtual bool encode(const QByteArray &data, QByteArray &output) = 0;
    virtual int getFrameSize() = 0;

protected:
    int m_samplerate;
    int m_channels;
    int m_bitrate;
};

#endif // BLEAUDIOENCODERABSTRACT_HPP
