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

#include "mstream.hpp"

#define E_SUCCESS           0
#define E_SPACE_NOT_ENOUGH  -2
#define E_STREAM_SKIP_ERROR -3

MStream::MStream()
    : m_pos(0)
{
}

void MStream::write1Bytes(uchar value)
{
    append(1, (char)value);
}

void MStream::write2Bytes(mint16 value)
{
    char *p = (char*)&value;
    append(1, p[1]);
    append(1, p[0]);
}

void MStream::write3Bytes(int value)
{
    char *p = (char*)&value;
    append(1, p[2]);
    append(1, p[1]);
    append(1, p[0]);
}

void MStream::write4Bytes(int value)
{
    char *p = (char*)&value;
    append(1, p[3]);
    append(1, p[2]);
    append(1, p[1]);
    append(1, p[0]);
}

void MStream::write8Bytes(mint64 value)
{
    char *p = (char*)&value;
    append(1, p[7]);
    append(1, p[6]);
    append(1, p[5]);
    append(1, p[4]);
    append(1, p[3]);
    append(1, p[2]);
    append(1, p[1]);
    append(1, p[0]);
}

void MStream::write8Bytes(double value)
{
    char *p = (char*)&value;
    append(1, p[7]);
    append(1, p[6]);
    append(1, p[5]);
    append(1, p[4]);
    append(1, p[3]);
    append(1, p[2]);
    append(1, p[1]);
    append(1, p[0]);
}

void MStream::writeString(const MString &value)
{
    append(value);
}

void MStream::writeString(const char *data, int size)
{
    append(data, size);
}

int MStream::read1Bytes(mint8 &var)
{
    int ret = E_SUCCESS;
    if (left() < (int)sizeof(var)) {
        return E_SPACE_NOT_ENOUGH;
    }

    char *p = (char*)&var;
    p[0] = at(m_pos++);

    return ret;
}

int MStream::read2Bytes(mint16 &var)
{
    int ret = E_SUCCESS;
    if (left() < 2) {
        return E_SPACE_NOT_ENOUGH;
    }

    char *p = (char*)&var;
    p[1] = at(m_pos++);
    p[0] = at(m_pos++);

    return ret;
}

int MStream::read3Bytes(mint32 &var)
{
    int ret = E_SUCCESS;
    if (left() < 3) {
        return E_SPACE_NOT_ENOUGH;
    }

    char *p = (char*)&var;
    p[3] = 0;
    p[2] = at(m_pos++);
    p[1] = at(m_pos++);
    p[0] = at(m_pos++);

    return ret;
}

int MStream::read4Bytes(mint32 &var)
{
    int ret = E_SUCCESS;
    if (left() < 4) {
        return E_SPACE_NOT_ENOUGH;
    }

    char *p = (char*)&var;
    p[3] = at(m_pos++);
    p[2] = at(m_pos++);
    p[1] = at(m_pos++);
    p[0] = at(m_pos++);

    return ret;
}

int MStream::read8Bytes(double &var)
{
    int ret = E_SUCCESS;
    if (left() < (int)sizeof(var)) {
        return E_SPACE_NOT_ENOUGH;
    }

    char *p = (char*)&var;
    p[7] = at(m_pos++);
    p[6] = at(m_pos++);
    p[5] = at(m_pos++);
    p[4] = at(m_pos++);
    p[3] = at(m_pos++);
    p[2] = at(m_pos++);
    p[1] = at(m_pos++);
    p[0] = at(m_pos++);

    return ret;
}

int MStream::readString(mint16 len, MString &var)
{
    int ret = E_SUCCESS;
    if (left() < (int)len) {
        return E_SPACE_NOT_ENOUGH;
    }

    var = substr(m_pos, len);
    m_pos += len;

    return ret;
}

int MStream::left()
{
    return size() - m_pos;
}

int MStream::skip(int len)
{
    int temp = m_pos;
    temp += len;
    if (temp >= (int)size() || temp < 0) {
        return E_STREAM_SKIP_ERROR;
    }
    m_pos = temp;

    return E_SUCCESS;
}

void MStream::reset()
{
    m_pos = 0;
}

bool MStream::end()
{
    return m_pos == (int)size();
}

int MStream::pos()
{
    return m_pos;
}

void MStream::print(int len)
{
    unsigned char *d = (unsigned char*)data() + m_pos;
    for (int i = 0; i < len; ++i) {
        printf("%02x  ", d[i]);
    }
    printf("\n");
    fflush(stdout);
}
