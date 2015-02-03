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

#ifndef BLEUTIL_H
#define BLEUTIL_H

#include <assert.h>
#include <QMutexLocker>

#include "BleLog.hpp"

#define BleFree(ptr) \
    do { if (ptr) { delete ptr; ptr = NULL;} } while (0)

#define BleFreeArray(ptr) \
    do { if (ptr) { delete [] ptr; ptr = NULL;} } while (0)

#define BleAutoLocker(mutex) QMutexLocker locker(&mutex)

#define BleAssert(x) do { assert(x); } while (0)

// from srs
template<class T>
class BleAutoFree
{
private:
    T **ptr;
    bool is_array;
public:
    BleAutoFree(T **_ptr, bool _is_array) {
        ptr = _ptr;
        is_array = _is_array;
    }

    virtual ~BleAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (is_array) {
            delete[] *ptr;
        } else {
            delete *ptr;
        }

        *ptr = NULL;
    }
};

#define BleAutoFree(className, instance) \
    BleAutoFree<className> BleAutoFree##instance(&instance, false)

#define BleAutoFreeArray(className, instance) \
    BleAutoFree<className> BleAutoFree##instance(&instance, true)

#define SAFE_STOP_THREAD(thread) \
    do { \
        if (thread && thread->isRunning()) { thread->stop(); thread->wait(); } \
    } while (0)

#define START_THREAD(thread) \
    do { \
        if (!thread->isRunning()) { thread->start(); } \
    } while (0)

QString formatS_size(qint64 bytes);

#endif // BLEUTIL_H
