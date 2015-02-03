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

#include "BleUtil.hpp"

#include <QLocale>

QString formatS_size(qint64 bytes)
{
    /// According to the Si standard KB is 1000 bytes, KiB is 1024
    /// but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;

    if (bytes >= tb)
        return QString(QObject::tr("%1 TB")).arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));

    if (bytes >= gb)
        return QString(QObject::tr("%1 GB")).arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));

    if (bytes >= mb)
        return QString(QObject::tr("%1 MB")).arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));

    if (bytes >= kb)
        return QString(QObject::tr("%1 KB")).arg(QLocale().toString(bytes / kb));

    return QString(QObject::tr("%1 B")).arg(QLocale().toString(bytes));
}
