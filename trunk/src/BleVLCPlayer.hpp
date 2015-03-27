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

    QImage getImage();

private:
    static void vlc_video_display_cb(void *opaque, void *picture);
    static void *vlc_video_lock_cb(void *opaque, void **planes);

    void addImage(QImage &image);

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
    QStringList m_options;
};

#endif // BLEVLCPLAYER_HPP
