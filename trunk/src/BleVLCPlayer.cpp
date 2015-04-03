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

#include "BleVLCPlayer.hpp"

#include <QDir>

#include "BleLog.hpp"
#include "BleErrno.hpp"
#include "BleUtil.hpp"
#include "BleAVContext.hpp"

#define DEFAULT_WIDTH       800
#define DEFAULT_HEIGHT      600
#define DEFAULT_SAMPLERATE  44100
#define DEFAULT_CHANNELS    2

BleVLCPlayer::BleVLCPlayer(QObject *parent) :
    QObject(parent)
  , m_type(MEDIA_TYPE_INVALID)
  , m_width(DEFAULT_WIDTH)
  , m_height(DEFAULT_HEIGHT)
  , m_sampleRate(DEFAULT_SAMPLERATE)
  , m_channels(DEFAULT_CHANNELS)
{

}

void BleVLCPlayer::setMediaType(int type)
{
    m_type = type;
}

void BleVLCPlayer::setMRL(const QString &mrl)
{
    m_mrl = mrl;
}

void BleVLCPlayer::setOptions(const QStringList &options)
{
    m_options = options;
}

void BleVLCPlayer::setAudioInfo(int sampleRate, int channels)
{
    m_sampleRate = sampleRate;
    m_channels = channels;
}

int BleVLCPlayer::start()
{
    if (m_mrl.isEmpty()) {
        log_error("start error(url is empty)");
        return BLE_VLC_MEDIA_MRL_EMPTY_ERROR;
    }

    if (m_type <= MEDIA_TYPE_INVALID || m_type >= MEDIA_TYPE_NUMBER) {
        log_error("start error(media type is error)");
        return BLE_VLC_MEDIA_TYPE_ERROR;
    }

    /*!
        \param argc the number of arguments (should be 0)
        \param argv list of arguments (should be NULL)
    */
    m_VLCInstance = libvlc_new(0, NULL);
    if (!m_VLCInstance) {
        log_error("libvlc_new error.");
        return BLE_VLC_INTERNAL_ERROR;
    }

    m_VLCPlayer = libvlc_media_player_new(m_VLCInstance);
    if (!m_VLCPlayer) {
        log_error("libvlc_media_player_new error.");
        libvlc_release(m_VLCInstance);
        return BLE_VLC_INTERNAL_ERROR;
    }

    if (m_type == MEDIA_TYPE_FILE) {
        const char *fileName = QDir::toNativeSeparators(m_mrl).toStdString().c_str();
        m_VLCMedia = libvlc_media_new_path(m_VLCInstance, fileName);
    } else if (m_type == MEDIA_TYPE_NETSTREAM) {
        const char *url = m_mrl.toStdString().c_str();
        m_VLCMedia = libvlc_media_new_location(m_VLCInstance, url);
//        libvlc_media_add_option(vlc_media, ":screen-fps=24");
//        libvlc_media_add_option(vlc_media, ":screen-top=0");
//        libvlc_media_add_option(vlc_media, ":screen-left=0");
//        libvlc_media_add_option(vlc_media, ":screen-width=800");
//        libvlc_media_add_option(vlc_media, ":screen-height=600");
//        libvlc_media_add_option(vlc_media, ":screen-follow-mouse");

//        vlc_media = libvlc_media_new_location(vlc_instance, "dshow:// ");
//        libvlc_media_add_option(vlc_media, ":dshow-vdev=6RoomsCamV9");
//        libvlc_media_add_option(vlc_media, ":dshow-fps=15");
//        libvlc_media_add_option(vlc_media, ":screen-left=0");
//        libvlc_media_add_option(vlc_media, ":screen-width=800");
//        libvlc_media_add_option(vlc_media, ":screen-height=600");
//        libvlc_media_add_option(vlc_media, ":screen-follow-mouse");
    }

    foreach (const QString &option, m_options) {
        libvlc_media_add_option(m_VLCMedia, option.toStdString().c_str());
    }

    if (!m_VLCMedia) {
        libvlc_media_player_release(m_VLCPlayer);
        libvlc_release(m_VLCInstance);
        return BLE_VLC_MEDIA_OPEN_ERROR;
    }

    libvlc_media_player_set_media(m_VLCPlayer, m_VLCMedia);

    // parse and get size
    libvlc_media_parse(m_VLCMedia);
    if (libvlc_video_get_size(m_VLCPlayer, 0, &m_width, &m_height) != 0) {
        log_warn("libvlc_video_get_size error. use default size %dx%d", DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_width = DEFAULT_WIDTH;
        m_height = DEFAULT_HEIGHT;
    }

    // @note RV24 equal to RGB24
    // param pitch bytes count per line.
    // chroma a four-characters string identifying the chroma
    // (e.g. "RV32" or "YUYV")
    // see vlc_fourcc.h
    // also see www.fourcc.org for detail four-characters color code.
    libvlc_video_set_format(m_VLCPlayer, "RV24", m_width, m_height, m_width*3);
    libvlc_video_set_callbacks(m_VLCPlayer, vlc_video_lock_cb, NULL, vlc_video_display_cb, this);

    // set audio callback
    libvlc_audio_set_format(m_VLCPlayer, "s16l", m_sampleRate, m_channels);
    libvlc_audio_set_callbacks(m_VLCPlayer, vlc_audio_playback_cb, NULL, NULL, NULL, NULL, this);

    // reg player
    BleAVContext::instance()->addPlayer(this);

    libvlc_media_player_play(m_VLCPlayer);

    return BLE_SUCESS;
}

void BleVLCPlayer::stop()
{
    if (libvlc_media_player_is_playing(m_VLCPlayer)) {
        libvlc_media_player_stop(m_VLCPlayer);
    }

    libvlc_media_release(m_VLCMedia);
    libvlc_media_player_release(m_VLCPlayer);
    libvlc_release(m_VLCInstance);

    BleAVContext::instance()->removePlayer(this);
}

void BleVLCPlayer::pause()
{
    void libvlc_media_player_pause ( libvlc_media_player_t *p_mi );
    libvlc_media_player_pause(m_VLCPlayer);
}

QImage BleVLCPlayer::getImage()
{
    BleAutoLocker(m_modifyMutex);
    return m_image;
}

int BleVLCPlayer::getAudioSamples(int frameSize, QByteArray &data)
{
    BleAutoLocker(m_audioMutex);

    if (m_samples.size() >= frameSize) {
        data = m_samples.mid(0, frameSize);
        m_samples.remove(0, frameSize);
    } else {
        return BLE_VLC_SAMPLE_NOT_ENOUGH;
    }

    return BLE_SUCESS;
}

void BleVLCPlayer::addImage(QImage &image)
{
    BleAutoLocker(m_modifyMutex);
    m_image = image;
}

void BleVLCPlayer::addSample(const void *samples, unsigned count, int64_t pts)
{
    BleAutoLocker(m_audioMutex);
    Q_UNUSED(pts);
    int size = count * 2 * m_channels;
    if (m_samples.size() >= size * 20) {
        m_samples.clear();
    }

    m_samples.append((char*)samples, size);
}

void BleVLCPlayer::vlc_video_display_cb(void *opaque, void *picture)
{
    BleVLCPlayer *source = (BleVLCPlayer *)opaque;
    int width = source->m_width;
    int height = source->m_height;

    QImage image(width, height, QImage::Format_RGB888);
    uchar *data = image.bits();
    memcpy(data, picture, image.byteCount());
    image = image.rgbSwapped();

    source->addImage(image);

    delete [] picture;
}

void *BleVLCPlayer::vlc_video_lock_cb(void *opaque, void **planes)
{
    BleVLCPlayer *source = (BleVLCPlayer *)opaque;
    int width = source->m_width;
    int height = source->m_height;

    int bytes = height * width * 3;
    char *pic_buffer = new char[bytes];;
    planes[0] = pic_buffer;

    return pic_buffer;
}

void BleVLCPlayer::vlc_audio_playback_cb(void *data, const void *samples
                                         , unsigned count, int64_t pts)
{
   BleVLCPlayer *player = (BleVLCPlayer *)data;
   if (!player) return;

   player->addSample(samples, count, pts);
}
