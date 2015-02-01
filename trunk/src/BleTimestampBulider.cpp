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

static double growTimestamp(double & timestamp, float internal, double & otherTimestamp)
{
    if (timestamp < otherTimestamp) {
        while (timestamp < otherTimestamp) {
            timestamp += internal;
        }
    } else {
       timestamp += internal;
    }

    return timestamp;
}

BleTimestampBulider::BleTimestampBulider()
    : m_videoInternal(66.66666666666667)        // default 15fps
    , m_audioInternal(23.2199546485261)          // default aac 44100Hz
    , m_videoTimestamp(0.00)
    , m_audioTimestamp(0.00)
{
}

void BleTimestampBulider::setVideoCaptureInternal(float internal)
{
    m_videoInternal = internal;
}

void BleTimestampBulider::setAudioCaptureInternal(float internal)
{
    m_audioInternal = internal;
}

double BleTimestampBulider::addVideoFrame()
{
    BleAutoLocker(m_mutex);

    if ((qint64)m_videoTimestamp <= (qint64)m_audioTimestamp) {
        while ((qint64)m_videoTimestamp <= (qint64)m_audioTimestamp) {
            m_videoTimestamp += (int)m_videoInternal;
        }

        return m_videoTimestamp;
    }

    if ((qint64)m_videoTimestamp > (qint64)m_audioTimestamp) {
        log_trace("video is too fast!");
        return m_videoTimestamp;
    }
}

double BleTimestampBulider::addAudioFrame(bool &need_capture_video, double &video_pts)
{
    BleAutoLocker(m_mutex);
    m_audioTimestamp += m_audioInternal;

    if (next_video_pts() - m_audioTimestamp < m_audioInternal) {
        need_capture_video = true;
        m_videoTimestamp = next_video_pts();
        video_pts = m_videoTimestamp;

        //log_trace("----->  V %lld  -----A %lld", (qint64)m_videoTimestamp, (qint64)m_audioTimestamp);
    } else {
        need_capture_video = false;
        video_pts = -1;
    }

    return m_audioTimestamp;
    // return growTimestamp(m_audioTimestamp, m_audioInternal, m_videoTimestamp);;
}

double BleTimestampBulider::next_video_pts()
{
    return m_videoTimestamp + m_videoInternal;
}
