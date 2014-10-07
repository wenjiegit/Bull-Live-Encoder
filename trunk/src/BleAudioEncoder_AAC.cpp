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

#include "BleAudioEncoder_AAC.hpp"
#include "BleLog.hpp"
#include "BleUtil.hpp"

BleAudioEncoder_AAC::BleAudioEncoder_AAC()
    : m_faacHandle(0)
    , m_outputBuffer(NULL)
{
    m_encoderType = AAC;
}

bool BleAudioEncoder_AAC::init(int samplerate, int channel, int bitrate)
{
    if (channel < 1 || channel > 6)
        return false;

    m_samplerate = samplerate;
    m_channels = channel;
    m_bitrate = bitrate;

    faacEncConfigurationPtr aacConfig;

    m_faacHandle = faacEncOpen(m_samplerate, m_channels, &m_samplesInputSize, &m_maxOutputSize);

    /* check faac version */
    aacConfig = faacEncGetCurrentConfiguration(m_faacHandle);
    if (aacConfig->version != FAAC_CFG_VERSION)
    {
        faacEncClose(m_faacHandle);
        log_error("faacEncGetCurrentConfiguration failed");
        return false;
    }

    // reserve reuseable output buffer
    m_outputBuffer = new unsigned char[m_maxOutputSize];

    // put the options in the configuration struct.
    aacConfig->aacObjectType = LOW; // MAIN
    aacConfig->mpegVersion = MPEG4;
    aacConfig->useTns = 1;
    aacConfig->useLfe = 1;
    aacConfig->allowMidside = 1;
    aacConfig->bitRate = m_bitrate / channel;
    aacConfig->bandWidth = 0;
    aacConfig->quantqual = 100;
    aacConfig->outputFormat = 0;
    aacConfig->inputFormat = FAAC_INPUT_16BIT;
    aacConfig->shortctl = SHORTCTL_NORMAL;

    if (!faacEncSetConfiguration(m_faacHandle, aacConfig))
    {
        log_error("faacEncSetConfiguration failed");
        return false;
    }

    m_nFrameSize = m_samplesInputSize / channel;
    m_pInBuf = new int32_t[m_samplesInputSize];

    /* Set decoder specific info */
    unsigned long extradata_size = 0;
    unsigned char *buffer;
    unsigned long decoder_specific_info_size;

    if (!faacEncGetDecoderSpecificInfo(m_faacHandle, &buffer, &decoder_specific_info_size)) {
            extradata_size = decoder_specific_info_size;
    }
    log_trace("aac encoder extradata_size=%d", extradata_size);

    m_header.clear();

    static unsigned char af[2] = {0xaf, 0x00};

    m_header.append((char *)af, 2);
    m_header.append((char*)buffer, extradata_size);

    BleFree(buffer);

    /*if (flags & CODEC_FLAG_GLOBAL_HEADER) {

        unsigned char *buffer;
        unsigned long decoder_specific_info_size;

        if (!faacEncGetDecoderSpecificInfo(m_FaacHandle, &buffer,
            &decoder_specific_info_size)) {
                extradata = buffer;
                extradata_size = decoder_specific_info_size;
        }
    }*/

    return true;
}

void BleAudioEncoder_AAC::fini()
{
    if (m_faacHandle) faacEncClose(m_faacHandle);

    BleFreeArray(m_pInBuf);
    BleFreeArray(m_outputBuffer);
}

bool BleAudioEncoder_AAC::encode(const QByteArray &data, QByteArray &outputArray)
{
    if (data.size() != getFrameSize()) {
        log_error("aac data is too not equa to %d", getFrameSize());
        return false;
    }

    // A negative value to indicate a failure, the number of vaid bytes
    // in the output buffer otherwise. A return value of zero does not
    // indicate failure.
    int encodedBytes = faacEncEncode(m_faacHandle, (int32_t*)data.data(), m_samplesInputSize, m_outputBuffer, m_maxOutputSize);
    log_info("aac ori data %d , encode data %d", data.size(), encodedBytes);
    if (encodedBytes < 0) {
        log_error("faacEncEncode failed, aac ori data %d , encode data %d", data.size(), encodedBytes);
        return false;
    }

    static unsigned char af[2] = {0xaf, 0x01};

    outputArray.append((char *)af, 2);
    outputArray.append((char*)m_outputBuffer, encodedBytes);

    return true;
}

int BleAudioEncoder_AAC::getFrameSize()
{
    return m_samplesInputSize * 16 / 8;
}

int BleAudioEncoder_AAC::getFrameDuration()
{
    return getFrameSize() * 1000 / (m_samplerate * 16 / 8 * 2);
}

QByteArray BleAudioEncoder_AAC::getHeader()
{
    return m_header;
}
