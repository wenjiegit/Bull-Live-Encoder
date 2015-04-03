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

#include "BleEncoderThread.hpp"
#include "BleX264Encoder.hpp"
#include "BleUtil.hpp"

#include <QMutexLocker>
#include <QFile>
#include <QDateTime>
#include <QElapsedTimer>
#include <QDebug>
#include <QQueue>

#include "BleRtmpMuxer.hpp"
#include "BleUtil.hpp"
#include "BleLog.hpp"
#include "BleImageProcessThread.hpp"

// opecv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "BleAVQueue.hpp"
#include "BleAVContext.hpp"

#define Max_Queue_Size     (20)

BleEncoderThread::BleEncoderThread(QObject *parent)
    : BleThread(parent)
    , m_x264Encoder(NULL)
    , m_imageProcessThread(NULL)
{
}

BleEncoderThread::~BleEncoderThread()
{
}

void BleEncoderThread::init()
{
    m_x264Encoder = new BleX264Encoder;
    m_x264Encoder->init();

    BleAVQueue::instance()->timestampBuilder()->
            setVideoCaptureInternal(m_x264Encoder->getFrameDuration());
}

void BleEncoderThread::fini()
{
    BleFree(m_x264Encoder);
}

void BleEncoderThread::run()
{
    while (!m_stop) {
        QQueue<BleImage*> images = BleAVContext::instance()->captureThread->getQueue();

        // if can't get image, then sleep 50 ms.
        if (images.isEmpty()) {
            msleep(5);
            continue;
        }

        while (!images.empty()) {
            BleImage * image = images.dequeue();
            BleAutoFree(BleImage, image);

            if (image->dataSize <= 0) continue;

            IplImage* imgYUV = cvCreateImage(cvSize(image->width, image->height * 3 / 2), IPL_DEPTH_8U, 1);
            IplImage *cvImage = cvCreateImageHeader(cvSize(image->width, image->height), IPL_DEPTH_8U, 3);
            cvImage->imageData = image->data;
            cvImage->imageDataOrigin = image->data;

            cvCvtColor(cvImage, imgYUV, CV_RGB2YUV_I420);

            m_x264Encoder->encode((uchar*)imgYUV->imageData, image->pts, image->opaque);

            cvReleaseImageHeader(&cvImage);
            cvReleaseImage(&imgYUV);

            if (m_stop) break;
        }

        // do clean
        for (int i = 0; i > images.size(); ++i) {
            BleImage *img = images.at(i);
            BleFree(img);
        }
    }

    log_trace("BleEncoderThread exit normally.");
}
