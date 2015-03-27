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

#ifndef BLEFILESOURCE_HPP
#define BLEFILESOURCE_HPP

#include "BleSourceAbstract.hpp"
#include "BleThread.hpp"

#include <QMutex>

class BleVLCPlayer;

class BleFileSource : public BleThread, public BleSourceAbstract
{
    Q_OBJECT
public:
    enum {
        MEDIA_TYPE_LOCAL_FILE = 0,
        MEDIA_TYPE_NET
    };

public:
    BleFileSource(QObject * parent = 0);

    virtual QString getSourceName();
    virtual QImage getImage();
    virtual void stopCapture();
    virtual void run();

    void setMedia(const QString &fileName);

private:
    static void vlc_video_display_cb(void *opaque, void *picture);
    static void *vlc_video_lock_cb(void *opaque, void **planes);

private:
    QString m_mediaName;
    BleVLCPlayer *m_vlcPlayer;
};

#endif // BLEFILESOURCE_HPP
