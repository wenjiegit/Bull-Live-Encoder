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

#include "BleLog.hpp"
#include "BleUtil.hpp"

#include <QDateTime>
#include <QDir>
#include <QMessageBox>

BleLog::BleLog()
    : m_logLevel(MLogLevel::Trace)
    , m_enableCache(false)
    , m_log2Console(true)
    , m_log2File(false)
    , m_enablFILE(false)
    , m_enableLINE(false)
    , m_enableFUNTION(true)
    , m_enableColorPrint(true)
    , m_file(NULL)
    , m_buffer(NULL)
    , m_bufferLength(4096)
    , m_timeFormat("MM-dd hh:mm:ss")
{
    m_buffer = new char[m_bufferLength];
}

BleLog::~BleLog()
{
    BleFree(m_file);
    BleFreeArray(m_buffer);
}

void BleLog::setLogLevel(int level)
{
    m_logLevel = level;
}

void BleLog::setEnableCache(bool enabled)
{
    m_enableCache = enabled;
}

void BleLog::setLog2Console(bool enabled)
{
    m_log2Console = enabled;
}

void BleLog::setLog2File(bool enabled)
{
    m_log2File = enabled;
    m_file = new QFile(m_filePath);
    if (!m_file->open(QIODevice::WriteOnly)) {
        return;
    }
}

void BleLog::setEnableFILE(bool enabled)
{
    m_enablFILE = enabled;
}

void BleLog::setEnableLINE(bool enabled)
{
    m_enableLINE = enabled;
}

void BleLog::setEnableFUNCTION(bool enabled)
{
    m_enableFUNTION = enabled;
}

void BleLog::setEnableColorPrint(bool enabled)
{
    m_enableColorPrint = enabled;
}

void BleLog::setTimeFormat(const MString &fmt)
{
    m_timeFormat = fmt;
}

void BleLog::setFilePath(const QString &path)
{
    QDir dir;
    dir.mkpath(path);
    m_filePath = QString("%1/%2")
            .arg(path)
            .arg(QDateTime::currentDateTime().toString("MM-dd-hh-mm-ss") + ".txt");
}

void BleLog::verbose(const char *file, muint16 line, const char *function
                     , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Verbose) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Verbose, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void BleLog::info(const char *file, muint16 line, const char *function
                  , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Info) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Info, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void BleLog::trace(const char *file, muint16 line, const char *function
                   , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Trace) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Trace, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void BleLog::warn(const char *file, muint16 line, const char *function
                  , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Warn) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Warn, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void BleLog::error(const char *file, muint16 line, const char *function
                   , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Error) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Error, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void BleLog::log(int level, const char *file, muint16 line, const char *function, const char *tag, const char *fmt, va_list ap)
{
    BleAutoLocker(m_mutex);

    const char *p;

    switch (level) {
    case MLogLevel::Verbose:
        p = "verbose";
        break;
    case MLogLevel::Info:
        p = "info";
        break;
    case MLogLevel::Trace:
        p = "trace";
        break;
    case MLogLevel::Warn:
        p = "warn";
        break;
    case MLogLevel::Error:
        p = "error";
        break;
    default:
        p = "default";
        break;
    }

    MString time = QDateTime::currentDateTime().toString(m_timeFormat.c_str()).toStdString();
    int size = 0;
    size += snprintf(m_buffer+size, m_bufferLength-size, "[%s][%d]", time.c_str(), 0);
    size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", p);

    if (m_enablFILE) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", file);
    }

    if (m_enableLINE) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%d]", line);
    }

    if (m_enableFUNTION) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", function);
    }

    if (tag) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", tag);
    }

    size += vsnprintf(m_buffer+size, m_bufferLength-size, fmt, ap);

#ifdef Q_OS_WIN
    size += snprintf(m_buffer+size, m_bufferLength-size, "\r\n");
#else
    size += snprintf(m_buffer+size, m_bufferLength-size, "\n");
#endif

    // log to console
    if (m_log2Console && m_enableColorPrint) {
        if (level <= MLogLevel::Trace) {
                  printf("\033[0m%s", m_buffer);
              } else if (level == MLogLevel::Warn) {
                  printf("\033[33m%s\033[0m", m_buffer);
              } else if (level == MLogLevel::Error){
                  printf("\033[31m%s\033[0m", m_buffer);
              }
    } else if (m_log2Console && !m_enableColorPrint) {
        printf("%s", m_buffer);
    }

    // log to file
    if (m_log2File) {
        m_file->write(m_buffer, size);
        m_file->flush();
    }

    fflush(stdout);
}

BleLog *g_logCtx = new BleLog;
