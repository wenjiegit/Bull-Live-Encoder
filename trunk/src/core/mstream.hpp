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

#ifndef MSTREAM_HPP
#define MSTREAM_HPP

#include "mstring.hpp"

class MStream : public MString
{
public:
    MStream();

    void write1Bytes(uchar value);
    void write2Bytes(mint16 value);
    void write3Bytes(int value);
    void write4Bytes(int value);
    void write8Bytes(mint64 value);
    void write8Bytes(double value);
    void writeString(const MString &value);
    void writeString(const char *data, int size);

    int read1Bytes(mint8 &var);
    int read2Bytes(mint16 &var);
    int read3Bytes(mint32 &var);
    int read4Bytes(mint32 &var);
    int read8Bytes(double &var);

    int readString(mint16 len, MString &var);

    int left();
    int skip(int len);
    void reset();
    bool end();
    int pos();
    void print(int len);

private:
    mint32 m_pos;
};

#endif // MSTREAM_HPP
