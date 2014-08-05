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

#include "mstringlist.hpp"

#include <iostream>

MStringList::MStringList()
{
}

MStringList::~MStringList()
{
}

bool MStringList::isEmpty() const
{
    return empty();
}

int MStringList::length()
{
    return size();
}

MString &MStringList::operator [](int i)
{
    list<MString>::iterator iter = begin();
    for (int c = 0; c < i; ++c) {
        ++iter;
    }
    return *iter;
}

const MString &MStringList::operator [](int i) const
{
    MStringListConstItor iter = begin();
    for (int c = 0; c < i; ++c) {
        ++iter;
    }
    return *iter;
}

MStringList &MStringList::operator <<(const MString &str)
{
    this->push_back(str);
    return *this;
}

MStringList &MStringList::operator <<(const MStringList &other)
{
    for (unsigned int i = 0; i < other.size(); ++i) {
        this->push_back(other.at(i));
    }

    return *this;
}

MStringList &MStringList::operator=(const MStringList &other)
{
    this->clear();
    return *this << other;
}

MString &MStringList::at(int pos)
{
    return (*this)[pos];
}

const MString &MStringList::at(int pos) const
{
    return (*this)[pos];
}

MString MStringList::join(const MString &sep)
{
    if (size() == 1) {
        return front();
    }

    MString ret;
    for (list<MString>::iterator iter = begin(); iter != end(); ++iter) {
        ret.append(*iter);
        ret.append(sep);
    }

    return ret;
}

MStringList MStringList::mid(int /*start*/, int /*length*/)
{
    // // TODO : imp
    MStringList ret;

    return ret;
}

ostream & operator<<(ostream &o, const MStringList& a)
{
    o << "(";
    for (unsigned int i = 0; i < a.size(); ++i) {
        const MString &str = a[i];
        o << "\"" << str << "\"";
        if (i < a.size() -1) {
            o << ",";
        }
    }
    o << ")";

    return o;
}
