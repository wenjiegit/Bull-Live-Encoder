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

#ifndef MSTRINGLIST_HPP
#define MSTRINGLIST_HPP

#include <list>
#include "mstring.hpp"

using namespace std;

class MStringList : public list<MString>
{
public:
    explicit MStringList();
    ~MStringList();

    bool isEmpty() const;
    int length();

    MString &operator[](int i);
    const MString& operator[](int i) const;

    MStringList &operator<<(const MString &str);
    MStringList &operator<<(const MStringList &other);
    MStringList &operator=(const MStringList &other);

    MString &at(int pos);
    const MString& at(int pos) const;

    MString join(const MString &sep);

    MStringList mid(int start, int length);

public:
    friend ostream & operator<<(ostream &o, const MStringList& a);
};

typedef MStringList::iterator MStringListItor;
typedef MStringList::const_iterator MStringListConstItor;

#endif // MSTRINGLIST_HPP
