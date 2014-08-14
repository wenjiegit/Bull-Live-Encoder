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

// opecv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

BlePictureSource::BlePictureSource(const QString &picName)
{
    IplImage * pImg = cvLoadImage(picName.toStdString().c_str());
    if (pImg) {
        BleImage be;
        be.width = pImg->width;
        be.height = pImg->height;

        be.data = new char[pImg->imageSize];
        memcpy(be.data, pImg->imageData, pImg->imageSize);

        be.dataSize = pImg->imageSize;
        be.format = BleImage_Format_BGR24;

        m_image = be;

        cvReleaseImage(&pImg);
    }
}

BleImage BlePictureSource::getImage()
{
    return m_image.clone();
}

void BlePictureSource::stopCapture()
{

}

void BlePictureSource::setCaptureInterval(int interval)
{
    Q_UNUSED(interval);
}
