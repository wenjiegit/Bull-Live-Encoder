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

#ifndef BLELOG_HPP
#define BLELOG_HPP

#include "mstring.hpp"

#include <stdarg.h>
#include <QFile>
#include <QMutex>

class MLogLevel
{
public:
    static const int Verbose    = 0x01;
    static const int Info       = 0x02;
    static const int Trace      = 0x04;
    static const int Warn       = 0x08;
    static const int Error      = 0x0F;
};

class BleLog
{
public:
    BleLog();
    ~BleLog();

    void setLogLevel(int level);

    void setEnableCache(bool enabled);
    void setLog2Console(bool enabled);
    void setLog2File(bool enabled);
    void setEnableFILE(bool enabled);
    void setEnableLINE(bool enabled);
    void setEnableFUNCTION(bool enabled);
    void setEnableColorPrint(bool enabled);

    void setTimeFormat(const MString &fmt);
    void setFilePath(const QString &path);

    virtual void verbose(const char *file, muint16 line, const char *function, const char *tag, const char* fmt, ...);
    virtual void info(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void trace(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void warn(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void error(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);

private:
    void log(int level, const char *file, muint16 line, const char *function, const char *tag, const char* fmt, va_list ap);

private:
    int m_logLevel;
    bool m_enableCache;
    bool m_log2Console;
    bool m_log2File;
    bool m_enablFILE;
    bool m_enableLINE;
    bool m_enableFUNTION;
    bool m_enableColorPrint;
    QFile *m_file;
    char *m_buffer;
    muint16 m_bufferLength;

    MString m_timeFormat;
    QString m_filePath;

    QMutex m_mutex;
};

extern BleLog *g_logCtx;

#define log_verbose(msg, ...) \
    g_logCtx->verbose(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_info(msg, ...) \
    g_logCtx->info(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_trace(msg, ...) \
    g_logCtx->trace(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_warn(msg, ...) \
    g_logCtx->warn(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_error(msg, ...) \
    g_logCtx->error(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#endif // BLELOG_HPP
