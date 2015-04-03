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

#ifndef BLEVLCPLAYER_HPP
#define BLEVLCPLAYER_HPP

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QImage>

#include "vlc/libvlc.h"
#include "vlc/libvlc_media.h"
#include "vlc/libvlc_media_player.h"
#include "BleSourceAbstract.hpp"

/*!
    @brief BleVLCPlayer is a play which is wraped with VLC SDK.
*/
class BleVLCPlayer : public QObject
{
public:
    enum {
        MEDIA_TYPE_INVALID,
        // for local file
        MEDIA_TYPE_FILE,
        // for stream based net.
        MEDIA_TYPE_NETSTREAM,
        // for capture device
        // for desktop, dshow(windows), v4l2(linux) ect.
        MEDIA_TYPE_CAPTURE,

        // the end
        MEDIA_TYPE_NUMBER
    };

    Q_OBJECT
public:
    explicit BleVLCPlayer(QObject *parent = 0);

    /*!
        set media type with @type
    */
    void setMediaType(int type);
    /*!
        set media resource url (which is mrl).
        for MEDIA_TYPE_FILE, this is  local file path.
        for MEDIA_TYPE_NETSTREAM, this is a stream url.
        for MEDIA_TYPE_CAPTURE, this is a name with option,
        eg: screen://  dshow://
    */
    void setMRL(const QString &mrl);
    /*!
        Add an option to the media.
        The options are listed in 'vlc --long-help' from the command line.
    */
    void setOptions(const QStringList &options);
    /*!
        set audio playback info
        @param sampleRate audio samplerate
        @param channels channels of audio
    */
    void setAudioInfo(int sampleRate, int channels);
    /*!
        start play media with mrl.
    */
    int start();
    /*!
        stop play media.
    */
    void stop();
    /*!
        pause play media.
    */
    void pause();

    /*!
        get video frame
    */
    QImage getImage();
    /*!
        get audio samples
        @return 0 could get @param bytes of samples.
    */
    int getAudioSamples(int frameSize, QByteArray &data);

private:
    static void vlc_video_display_cb(void *opaque, void *picture);
    static void *vlc_video_lock_cb(void *opaque, void **planes);
    static void vlc_audio_playback_cb(void *data, const void *samples,
                                      unsigned count, int64_t pts);

    void addImage(QImage &image);
    void addSample(const void *samples, unsigned count, int64_t pts);

private:
    QString m_mrl;
    int m_type;
    unsigned int m_width;
    unsigned int m_height;
    libvlc_instance_t *m_VLCInstance;
    libvlc_media_player_t *m_VLCPlayer;
    libvlc_media_t *m_VLCMedia;
    QImage m_image;
    QMutex m_modifyMutex;
    QMutex m_audioMutex;
    QStringList m_options;
    QByteArray m_samples;
    int m_sampleRate;
    int m_channels;
};

#endif // BLEVLCPLAYER_HPP
