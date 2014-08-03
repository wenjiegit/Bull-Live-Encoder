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

#include "BleTimestampBulider.hpp"

#include "BleUtil.hpp"

static qint64 growTimestamp(qint64 & timestamp, int internal, qint64 & otherTimestamp)
{
    if (timestamp <= otherTimestamp) {
        while (timestamp <= otherTimestamp) {
            timestamp += internal;
        }
    } else {
       timestamp += internal;
    }

    return timestamp;
}

BleTimestampBulider::BleTimestampBulider()
    : m_videoInternal(50)
    , m_audiInternal(23)
    , m_videoTimestamp(0)
    , m_audioTimestamp(0)
{
}

void BleTimestampBulider::setVideoCaptureInternal(int internal)
{
    m_videoInternal = internal;
}

void BleTimestampBulider::setAudioCaptureInternal(int internal)
{
    m_audiInternal = internal;
}

qint64 BleTimestampBulider::addVideoFrame()
{
    BleAutoLocker(m_mutex);

    qint64 ret = 0;
    if (m_videoTimestamp == 0) {
        ret = 0;
        m_videoTimestamp += m_videoInternal;
    } else {
        ret = growTimestamp(m_videoTimestamp, m_videoInternal, m_audioTimestamp);
    }

    return ret;
}

qint64 BleTimestampBulider::addAudioFrame()
{
    BleAutoLocker(m_mutex);

    qint64 ret = 0;
    if (m_audioTimestamp == 0) {
        ret = 0;
        m_audioTimestamp += m_audiInternal;
    } else {
        ret = growTimestamp(m_audioTimestamp, m_audiInternal, m_videoTimestamp);
    }

    return ret;
}
