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

#ifndef BLEIMAGEPROCESSTHREAD_HPP
#define BLEIMAGEPROCESSTHREAD_HPP

#include "BleThread.hpp"
#include "BleSourceAbstract.hpp"
#include "BleTimestampBulider.hpp"

#include <QHash>
#include <QRect>
#include <QMutex>
#include <QQueue>

// opecv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

class BleSourceAbstract;
struct BleImage;

class BleImageProcessThread : public BleThread
{
    Q_OBJECT

public:
    BleImageProcessThread(QObject * parent = 0);

    virtual void run();

    void setResolution(int w, int h);
    void updateSources(QList<SourcePair> & sources);
    void setInternal(int internal);
    BleImage *getImage();

private:
    int m_width;
    int m_height;

    QMutex m_updateMutex;

    QQueue<BleImage *> m_outputQueue;
    QList<SourcePair> m_sources;
    int m_internal;

    QImage *m_image;
};

#endif // BLEIMAGEPROCESSTHREAD_HPP
