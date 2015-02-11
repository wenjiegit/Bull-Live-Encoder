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

#ifndef MOption_H
#define MOption_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QMutex>
#include <QHash>

enum interfaceOptions
{
    OPTION_WindowBGPixmap,
    OPTION_WindowBGColor,
    OPTION_AeroTransparent,
    OPTION_WidgetTransparent,
    OPTION_Recommand,
    OPTION_WindowBGPixmapType,
    OPTION_RemeberPasswd,
    OPTION_AutoLogin,
    OPTION_NOTREMIND,
    OPTION_QUITORHIDE,
    OPTION_MINTOTRAY,
    OPTION_SHORTCUTS,
    OPTION_MainWindowBootAutoLogin,
    OPTION_USER_PASS,
    OPTION_AVERAGE_COLOR,
    OPTION_REFRESH_AUTO,
    OPTION_REFRESH_BY_HAND,
    OPTION_REFRESH_TIME,

    OPTIONS_NUM
};

enum interfaceOptions_Group
{
    OPTION_GROUP_Main,
    OPTION_GROUP_Theme,

    OPTIONS_GROUP_NUM
};

#define Key_Thread_Count    "ThreadCount"
#define Key_Enable_B_Frame  "EnableBFrame"
#define Key_B_Frame_Count  "b_frame_count"

#define Group_X264          "x264"

class MOption : public QObject
{
    Q_OBJECT
protected:
    explicit MOption(QObject *parent = 0);
    ~MOption();

public:
    QVariant option(const QString &key, const QString &group);
    QVariant option(const int &key, const int &group);
    void setOption(const QVariant& value, const QString &key, const QString &group);
    void setOption(const QVariant& value, const int &key, const int &group);

    static MOption* instance();
    static void destroy();

protected:
    void setDefaultValue();
    QVariant defaultValue(const QString &key);

private:
    static MOption *m_theOptions;

    QSettings *m_settings;
    QHash<QString, QVariant> m_defaultValue;

    QMutex m_mutex;

};

#endif // MOption_H
