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

#include "mstring.hpp"

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "mstringlist.hpp"

MString::MString()
{
}

MString::MString(const char *str)
    : string(str)
{

}

MString::MString(const char *str, int n)
    : string(str, n)
{

}

MString::MString(const string &str)
    : string(str)
{

}

//MString::MString(const string &str, size_t pos, size_t len)
//    : string(str, pos, len)
//{

//}

MString::MString(size_t n, char c)
    : string(n, c)
{

}

MString::~MString()
{
}

string MString::toStdString()
{
    return string(c_str());
}

void MString::chop(int n)
{
    if ((muint32)n >= size()) {
        clear();

        return;
    }

    erase(size() - n, n);
}

// '\t', '\n', '\v', '\f', '\r', and ' '
MString MString::trimmed() const
{
    int beginIndex = 0;
    int endIndex = 0;

    // find beginIndex
    for (unsigned int i = 0; i < size(); ++i) {
        char c = this->at(i);
        if (c == '\t'
                || c == '\n'
                || c == '\v'
                || c == '\f'
                || c == '\r'
                || c == ' ') {
            ++beginIndex;
        } else {
            break;
        }
    }

    // find endIndex
    for (int i = size() - 1; i >= 0; --i) {
        char c = this->at(i);
        if (c == '\t'
                || c == '\n'
                || c == '\v'
                || c == '\f'
                || c == '\r'
                || c == ' ') {
            ++endIndex;
        } else {
            break;
        }
    }

    return substr(beginIndex, size() - beginIndex - endIndex);
}

void MString::truncate(int position)
{
    MStringIterator first = begin() + position;
    erase(first, end());
}

MString &MString::sprintf(const char *cformat, ...)
{
    va_list vp;
    va_start(vp, cformat);

    char buffer[1024];
    vsprintf(buffer, cformat, vp);
    va_end(vp);
    this->append(buffer);

    return *this;
}

bool MString::contains(const MString &str) const
{
    return find(str) != npos;
}

bool MString::contains(const string &str) const
{
    return find(str) != npos;
}

bool MString::contains(const char *str) const
{
    return find(str) != npos;
}

bool MString::startWith(const MString &str) const
{
    return find_first_of(str) == 0;
}

bool MString::startWith(const string &str) const
{
    return find_first_of(str) == 0;
}

bool MString::startWith(const char *str) const
{
    return find_first_of(str) == 0;
}

bool MString::endWith(const MString &str) const
{
    return find_last_of(str) == (size() - str.size());
}

bool MString::endWith(const string &str) const
{
    return find_last_of(str) == (size() - str.size());
}

bool MString::endWith(const char *str) const
{
    return find_last_of(str) == (size() - strlen(str));
}

MString &MString::replace(const MString &before, const MString &after, bool replaceAll)
{
    size_t  pos = npos;
    while ((pos = find(before.c_str())) != npos) {
        string::replace(pos, before.size(), after);

        if (!replaceAll) {
            break;
        }
    }

    return *this;
}


MStringList MString::split(const MString &sep)
{
    MString temp(*this);
    MStringList ret;
    if (sep.isEmpty()) {
        return ret;
    }

    while (temp.contains(sep)) {
        size_type index = temp.find(sep);

        MString ss = temp.substr(0, index);
        if (!ss.isEmpty()) {
            ret << ss;
        }
        temp = temp.substr(index + sep.size(), temp.size() - 1);
    }
    if (!temp.isEmpty()) {
        ret << temp;
    }

    return ret;
}

MString &MString::prepend(const MString &str)
{
    MString temp = *this;
    *this = str + temp;
    return *this;
}

MString &MString::prepend(const char *str, int size)
{
    MString temp(str, size);
    return prepend(temp);
}

MString & MString::operator <<(mint32 value)
{
    append(number(value));
    return *this;
}

MString & MString::operator <<(muint64 value)
{
    append(number(value));
    return *this;
}

MString & MString::operator <<(const MString &str)
{
    append(str);
    return *this;
}

MString MString::number(mint32 n, int /*base*/)
{
    // TODO process base
    char buffer[32] = {0};
    ::sprintf(buffer, "%d", n);

    return buffer;
}

MString MString::number(muint64 n, int /*base*/)
{
    // TODO process base
    char buffer[64] = {0};
    ::sprintf(buffer, "%lld", n);

    return buffer;
}

MString MString::number(size_t n, int /*base*/)
{
    // TODO process base
    char buffer[32] = {0};
    ::sprintf(buffer, "%d", n);

    return buffer;
}
