#include "BleAudioEncoder_AAC.hpp"
#include "BleLog.hpp"
#include "BleUtil.hpp"

BleAudioEncoder_AAC::BleAudioEncoder_AAC()
    : m_faacHandle(0)
{
}

bool BleAudioEncoder_AAC::init(int samplerate, int channel, int bitrate)
{
    faacEncConfigurationPtr faac_cfg;

    if (channel < 1 || channel > 6)
        return false;

    m_channels = channel;

    m_faacHandle = faacEncOpen(samplerate, channel, &m_nSamplesInput, &m_nMaxbytesOutput);

    /* check faac version */
    faac_cfg = faacEncGetCurrentConfiguration(m_faacHandle);
    if (faac_cfg->version != FAAC_CFG_VERSION)
    {
        faacEncClose(m_faacHandle);
        log_error("faacEncGetCurrentConfiguration failed");
        return false;
    }
    log_trace("m_nSamplesInput = %d, m_nMaxbytesOutput = %d", m_nSamplesInput, m_nMaxbytesOutput);

    /* put the options in the configuration struct */
    faac_cfg->aacObjectType = LOW; // MAIN
    faac_cfg->mpegVersion = MPEG4;
    faac_cfg->useTns = 1;                       // 时域噪声整形（TNS）
    faac_cfg->useLfe = 1;
    faac_cfg->allowMidside = 1;
    faac_cfg->bitRate = bitrate / channel;
    faac_cfg->bandWidth = 0;
    faac_cfg->quantqual = 100;
    faac_cfg->outputFormat = 1;                 // 1:包含ADTS头
    faac_cfg->inputFormat = FAAC_INPUT_16BIT;
    faac_cfg->shortctl = SHORTCTL_NORMAL;

    if (!faacEncSetConfiguration(m_faacHandle, faac_cfg))
    {
        log_error("faacEncSetConfiguration failed");
        return false;
    }

    m_nFrameSize = m_nSamplesInput / channel;
    m_pInBuf = new int32_t[m_nSamplesInput];

    /* Set decoder specific info */
    unsigned long extradata_size = 0;
    unsigned char *buffer;
    unsigned long decoder_specific_info_size;

    if (!faacEncGetDecoderSpecificInfo(m_faacHandle, &buffer, &decoder_specific_info_size)) {
            extradata_size = decoder_specific_info_size;
    }
    log_trace("aac encoder extradata_size=%d", extradata_size);

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
}

bool BleAudioEncoder_AAC::encode(const QByteArray &data, QByteArray &output)
{
//    int bytes_written = faacEncEncode(m_faacHandle, (int32_t*)data.data(), m_nSamplesInput,
//        (unsigned char*)pOutput, m_nMaxbytesOutput);

//    nOutput = bytes_written;

    return true;
}
