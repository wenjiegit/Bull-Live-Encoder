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

#include "BlePictureSource.hpp"

#include <QMovie>

#include "BleUtil.hpp"
#include "BleLog.hpp"

BlePictureSource::BlePictureSource(const QString &picName)
{
    m_movie = NULL;

    if(picName.endsWith(".gif") || picName.endsWith(".GIF")) {
        m_movie = new QMovie(picName, "gif", this);
        m_movie->setCacheMode(QMovie::CacheAll);
        connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged(int)));
        m_movie->start();
    } else {
        m_image = QImage(picName);
    }
}

BlePictureSource::~BlePictureSource()
{

}

QString BlePictureSource::getSourceName()
{
    return "BlePictureSource";
}

QImage BlePictureSource::getImage()
{
    BleAutoLocker(m_mutex);
    return m_image;
}

void BlePictureSource::stopCapture()
{

}

void BlePictureSource::setCaptureInterval(int interval)
{
    Q_UNUSED(interval);
}

void BlePictureSource::onFrameChanged(int frameNumber)
{
    Q_UNUSED(frameNumber);
    BleAutoLocker(m_mutex);
    m_image = m_movie->currentImage();
}
