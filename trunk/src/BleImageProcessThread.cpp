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

#include "BleImageProcessThread.hpp"

#include "BleLog.hpp"
#include "BleSourceAbstract.hpp"
#include "BleUtil.hpp"
#include "BleAVUtil.hpp"
#include "BleAVQueue.hpp"

#include <QElapsedTimer>
#include <QImage>
#include <QPainter>

static const int ImageProcess_Default_Width  = 640;
static const int ImageProcess_Default_Height = 480;
static const int ImageProcess_Default_Internal = 50;        // 20 fps

BleImageProcessThread::BleImageProcessThread(QObject *parent)
    : BleThread(parent)
    , m_width(ImageProcess_Default_Width)
    , m_height(ImageProcess_Default_Height)
    , m_internal(ImageProcess_Default_Internal)
{
}

void BleImageProcessThread::run()
{
    m_image = new QImage(m_width, m_height, QImage::Format_RGB888);
    QPainter p(m_image);

    // TODO make this to option
    p.setRenderHints(QPainter::SmoothPixmapTransform);

    while (!m_stop) {
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        m_updateMutex.lock();
        // reset bg image to black
        uchar *imageData = m_image->bits();
        int imageSize = m_image->byteCount();
        memset(imageData, 0, imageSize);

        for (int i = 0; i < m_sources.size(); ++i) {
            SourcePair & pair = m_sources[i];

            QImage sourceImage = pair.source->getImage();
            p.drawImage(pair.rect, sourceImage);
        }
#if 0
        static IplImage *image=  cvLoadImage("bbs.png", CV_LOAD_IMAGE_UNCHANGED);
        if (image) {
            for (int x = 0; x < image->width; ++x) {
                for (int y = 0; y < image->height; ++y) {

                    //unsigned char *start = image->imageData + y*image->widthStep + x * 4;

                    unsigned char B = (image->imageData + y*image->widthStep)[x * 4 + 0];
                    unsigned char G = (image->imageData + y*image->widthStep)[x * 4 + 1];
                    unsigned char R = (image->imageData + y*image->widthStep)[x * 4 + 2];
                    unsigned char A = (image->imageData + y*image->widthStep)[x * 4 + 3];


                    if (A == 0) continue;

                    int newX = x + 40;
                    int newY = y + 30;

                    float A1 = A/255.00;

                    unsigned char B1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 0];
                    unsigned char G1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 1];
                    unsigned char R1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 2];

                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 0] = (1-A1)*B1 + A1*B;
                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 1] = (1-A1)*G1 + A1*G;
                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 2] = (1-A1)*R1 + A1*R;
                }
             }
        }
#endif

        m_updateMutex.unlock();

        int elapsedMs = elapsedTimer.elapsed();
        int needSleepMs = m_internal - elapsedMs;
        if (needSleepMs < 0) {
            needSleepMs = 0;
        }
        msleep(needSleepMs);
    }

    log_trace("BleImageProcessThread exit normally.");
}

void BleImageProcessThread::setResolution(int w, int h)
{
    m_width = w;
    m_height = h;
}

void BleImageProcessThread::updateSources(QList<SourcePair> &sources)
{
    BleAutoLocker(m_updateMutex);
    m_sources = sources;
}

void BleImageProcessThread::setInternal(int internal)
{
    m_internal = internal;
}

BleImage *BleImageProcessThread::getImage()
{
    if (!m_image) return NULL;

    BleAutoLocker(m_updateMutex);

    // to BleImage
    BleImage *be = new BleImage;
    be->width = m_image->width();
    be->height = m_image->height();
    be->dataSize =m_image->byteCount();

    be->data = new char[be->dataSize];
    memcpy(be->data, m_image->bits(), be->dataSize);

    be->format = BleImage_Format_BGR24;

    return be;
}
