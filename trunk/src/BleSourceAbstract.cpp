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

#include "BleSourceAbstract.hpp"

#include "BleUtil.hpp"

BleImage::BleImage()
{
    init();
}

BleImage::~BleImage()
{
    if (deRef() <= 0) {
        BleFreeArray(data);
        BleFree(ref);
    }
}

BleImage &BleImage::operator=(BleImage &other)
{
    // if has data alloc and no ref, first free it.
    if (deRef() <= 0) {
        BleFreeArray(data);
        BleFree(ref);
    }

    this->pts = other.pts;
    this->width = other.width;
    this->height = other.height;
    this->format = other.format;
    this->dataSize = other.dataSize;
    this->data = other.data;
    this->ref = other.ref;

    other.addRef();

    return *this;
}

BleImage::BleImage(const BleImage &other)
{
    this->pts = other.pts;
    this->width = other.width;
    this->height = other.height;
    this->format = other.format;
    this->dataSize = other.dataSize;
    this->data = other.data;
    this->ref = other.ref;

    this->addRef();
}

BleImage BleImage::clone()
{
    BleImage image;
    image.width = width;
    image.height = height;
    image.pts = pts;
    image.format = format;

    image.data = new char[dataSize];
    memcpy(image.data, data, dataSize);

    image.dataSize = dataSize;

    return image;
}

int BleImage::addRef()
{
    *ref += 1;

    return *ref;
}

int BleImage::deRef()
{
    *ref -= 1;

    return *ref;
}

void BleImage::init()
{
    width = 0;
    height = 0;
    format = BleImage_Format_Invalid;
    dataSize = 0;
    data = NULL;
    pts = 0;
    opaque = NULL;

    ref = new int(0);

    addRef();
}

BleSourceAbstract::BleSourceAbstract()
{
}

BleSourceAbstract::~BleSourceAbstract()
{

}

QString BleSourceAbstract::getSourceName()
{
    return "BleSourceAbstract";
}

QImage BleSourceAbstract::getImage()
{
    return QImage();
}

void BleSourceAbstract::stopCapture()
{

}

void BleSourceAbstract::setCaptureInterval(int interval)
{
    Q_UNUSED(interval);
}


