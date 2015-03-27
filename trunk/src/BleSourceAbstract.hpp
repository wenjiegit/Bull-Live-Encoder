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

#ifndef BLESOURCEABSTRACT_HPP
#define BLESOURCEABSTRACT_HPP

#include <QString>
#include <QRect>
#include <QImage>

#define BleImage_Format_Invalid         -1
#define BleImage_Format_RGB24           0x01
#define BleImage_Format_BGR24           0x02
#define BleImage_Format_RGB32           0x03
#define BleImage_Format_YUV420          0x04
#define BleImage_Format_RGBA            0x05

struct BleImage
{
    qint64 pts;
    int width;
    int height;

    int format;

    int dataSize;
    char *data;
    int *ref;

    void *opaque;

    BleImage();
    ~BleImage();

    BleImage & operator=(BleImage & other);
    BleImage(const BleImage & other);

    BleImage clone();

    int addRef();
    int deRef();

    void destory();    

private:
    void init();
};

class BleSourceAbstract;
struct SourcePair {
    BleSourceAbstract *source;
    QRect rect;

    bool operator ==(const SourcePair & other) {
        return source == other.source;
    }
};

class BleSourceAbstract
{
public:
    BleSourceAbstract();
    virtual ~BleSourceAbstract();

    virtual QString getSourceName();
    virtual QImage getImage();
    virtual void stopCapture();
    virtual void setCaptureInterval(int interval);
};

#endif // BLESOURCEABSTRACT_HPP
