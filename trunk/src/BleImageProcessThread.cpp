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

#include <QDateTime>
#include <QElapsedTimer>
#include <QElapsedTimer>

static const int ImageProcess_Default_Width  = 320;
static const int ImageProcess_Default_Height = 240;
static const int ImageProcess_Default_Internal = 50;        // 20 fps
static CvRect QRect2CvRect(const QRect &rect)
{
    CvRect ret;
    ret.x = rect.x();
    ret.y = rect.y();
    ret.width = rect.width();
    ret.height = rect.height();

    return ret;
}

BleImageProcessThread::BleImageProcessThread(QObject *parent)
    : BleThread(parent)
    , m_width(ImageProcess_Default_Width)
    , m_height(ImageProcess_Default_Height)
    , m_internal(ImageProcess_Default_Internal)
    , m_dstImage(NULL)
{
}

void BleImageProcessThread::run()
{
    CvSize dstSize;
    dstSize.width = m_width;
    dstSize.height = m_height;
    IplImage* dstImg = cvCreateImage(dstSize, IPL_DEPTH_8U, 3);
    cvZero(dstImg);
    m_dstImage = dstImg;

    QRect dstRect(0, 0, m_width, m_height);

    while (!m_stop) {
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        m_updateMutex.lock();
        for (int i = 0; i < m_sources.size(); ++i) {
            SourcePair & pair = m_sources[i];

            BleImage bleImage = pair.source->getImage();
            if (bleImage.dataSize <= 0) continue;

            IplImage *cvImage = cvCreateImageHeader(cvSize(bleImage.width, bleImage.height), 8, 3);
            cvImage->imageData = bleImage.data;
            cvImage->imageDataOrigin = bleImage.data;

            // if the image size not qual to area size
            // then resize it.
            IplImage *resizedImage = cvImage;
            if (cvImage->width != pair.rect.width() ||
                    cvImage->height != pair.rect.height())
            {
                resizedImage = cvCreateImage(cvSize(pair.rect.width(), pair.rect.height()), cvImage->depth, cvImage->nChannels);
                cvResize(cvImage, resizedImage, CV_INTER_LINEAR);
            }

            if (bleImage.format == BleImage_Format_RGB24) {
                cvConvertImage(resizedImage, resizedImage, CV_CVTIMG_SWAP_RB);
            }

            // get the intersection of dst rect
            if (!dstRect.intersects(pair.rect)) continue;

            QRect intersectionRect_1 = dstRect.intersected(pair.rect);
            QRect intersectionRect_2 = pair.rect.intersected(dstRect);

            // intersectionRect_2 should relative to pair.rect
            intersectionRect_2.moveTopLeft(QPoint(intersectionRect_2.x() - pair.rect.x(),
                                       intersectionRect_2.y() - pair.rect.y()));

            cvSetImageROI(dstImg, QRect2CvRect(intersectionRect_1));
            cvSetImageROI(resizedImage, QRect2CvRect(intersectionRect_2));

            cvCopy(resizedImage, dstImg);

            cvResetImageROI(dstImg);
            cvResetImageROI(resizedImage);

            // if resizedImage is cvCreateImage created ?
            if (resizedImage != cvImage) {
                cvReleaseImage(&resizedImage);
            }
            cvReleaseImageHeader(&cvImage);
        }

//        IplImage *image=  cvLoadImage("d:\\123.png", CV_LOAD_IMAGE_UNCHANGED);
//        if (image) {
//            for (int x = 0; x < image->width; ++x) {
//                for (int y = 0; y < image->height; ++y) {

//                    //unsigned char *start = image->imageData + y*image->widthStep + x * 4;

//                    unsigned char B = (image->imageData + y*image->widthStep)[x * 4 + 0];
//                    unsigned char G = (image->imageData + y*image->widthStep)[x * 4 + 1];
//                    unsigned char R = (image->imageData + y*image->widthStep)[x * 4 + 2];
//                    unsigned char A = (image->imageData + y*image->widthStep)[x * 4 + 3];


//                    if (A == 0) continue;

//                    int newX = x + 400;
//                    int newY = y + 30;

//                    float A1 = A/255.00;

//                    unsigned char B1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 0];
//                    unsigned char G1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 1];
//                    unsigned char R1 = ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 2];

//                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 0] = (1-A1)*B1 + A1*B;
//                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 1] = (1-A1)*G1 + A1*G;
//                    ((dstImg->imageData + newY*dstImg->widthStep))[newX*3 + 2] = (1-A1)*R1 + A1*R;
//                }
//             }
//        }


        m_updateMutex.unlock();

        m_modifyOutputMutex.lock();

        // if delayed about 1s , then discard some image.
        // TODO make this to option
        // TODO check AVQueue size
        if (m_outputQueue.size() > 5) {
            log_trace("queue has many mang image, maybe your encoder is too slow!");
            goto end;
        }

        if (true) {
            // to BleImage
            BleImage *be = new BleImage;
            be->width = dstImg->width;
            be->height = dstImg->height;

            be->data = new char[dstImg->imageSize];
            memcpy(be->data, dstImg->imageData, dstImg->imageSize);

            be->dataSize = dstImg->imageSize;

            be->format = BleImage_Format_BGR24;

            // m_timestampBuilder.setVideoCaptureInternal(m_internal);
            // be->pts = m_timestampBuilder.addVideoFrame();

            BleVideoPacket *pkt = new BleVideoPacket(Video_Type_H264);
            pkt->ready = false;
            pkt->dts = be->pts = BleAVQueue::instance()->timestampBuilder()->addVideoFrame();
            BleAVQueue::instance()->enqueue(pkt);

            m_outputQueue.enqueue(be);
        }

end:
        m_modifyOutputMutex.unlock();

        int elapsedMs = elapsedTimer.elapsed();
        int needSleepMs = m_internal - elapsedMs;
        if (needSleepMs < 0) {
            needSleepMs = 0;
        }
        msleep(needSleepMs);

        // reset bg image to black
        cvZero(dstImg);
    }
    m_dstImage = NULL;
    cvReleaseImage(&dstImg);

    // clean queue
    for (int i = 0; i < m_outputQueue.size(); ++i) {
        BleImage *img = m_outputQueue.at(i);
        BleFree(img);
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
    if (!m_dstImage) return NULL;

    BleAutoLocker(m_updateMutex);

    // to BleImage
    BleImage *be = new BleImage;
    be->width = m_dstImage->width;
    be->height = m_dstImage->height;

    be->data = new char[m_dstImage->imageSize];
    memcpy(be->data, m_dstImage->imageData, m_dstImage->imageSize);

    be->dataSize = m_dstImage->imageSize;

    be->format = BleImage_Format_BGR24;

    return be;
}

QQueue<BleImage *> BleImageProcessThread::getQueue()
{
    BleAutoLocker(m_modifyOutputMutex);

    QQueue<BleImage *> queue = m_outputQueue;
    m_outputQueue.clear();

    return queue;
}
