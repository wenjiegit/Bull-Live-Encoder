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

#include "MOption.h"

#include <QCoreApplication>
#include <QStringList>
#include <QColor>
#include <QDir>

struct t_Option
{
    QString name;
    QVariant defaultValue;
};

struct t_Option_Group
{
    QString name;
};

QString path = QDir::currentPath() + "/theme/saved/recommand/bg2.png";
static const t_Option options[OPTIONS_NUM] =
{
    {"WindowBGPixmap", path},
    {"WindowBGColor", QColor(100,178,226,255)},
    {"AeroTransparent", 251},
    {"WidgetTransparent", 252},
    {"Recommand", QStringList() << "theme/saved/recommand/bg1.png" << "theme/saved/recommand/bg2.png" << "theme/saved/recommand/bg3.png" << "theme/saved/recommand/bg4.png"},
    {"WindowBGPixmapType", "bitmap"},
    {"RemeberPasswd", 0},
    {"AutoLogin", 0},
    {"NOTREMIND", 0},
    {"QUITORHIDE", "QuitApp"},
    {"MINTOTRAY", 0},
    {"SHORTCUTS", "Ctrl + H"},
    {"MainWindowBootAutoLogin", 0},
    {"last login", QStringList()},
    {"AverageColor", QColor(100,178,226,255)},
    {"Refresh_Auto", Qt::Unchecked},
    {"Refresh_By_Hand", Qt::Checked},
    {"Refresh_TIME", 5},

};

static const t_Option_Group options_Group[OPTIONS_GROUP_NUM] =
{
    {"Main"},
    {"Theme"}
};

/// define ini file name and path
#define SETTING_FILE "/setting.ini"
MOption* MOption::m_theOptions = 0;

MOption::MOption(QObject *parent) :
    QObject(parent)
{
    QString settingsPath = QCoreApplication::applicationDirPath() + SETTING_FILE;
    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    setDefaultValue();
}

MOption::~MOption()
{
    delete m_settings;

    destroy();
}

MOption* MOption::instance()
{
    if(!m_theOptions)
        m_theOptions = new MOption;

    return m_theOptions;
}

void MOption::destroy()
{
    if(m_theOptions)
    {
        delete m_theOptions;
        m_theOptions = 0;
    }
}

void MOption::setDefaultValue()
{
    for(int i = 0; i < OPTIONS_NUM; ++i)
    {
        m_defaultValue.insert(options[i].name, options[i].defaultValue);
    }
}

QVariant MOption::defaultValue(const QString &key)
{
    return m_defaultValue.value(key, QVariant());
}

void MOption::setOption(const QVariant &value, const QString &key, const QString &group)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
    m_settings->sync();
}

void MOption::setOption(const QVariant& value, const int &key, const int &group)
{
    setOption(value, options[key].name, options_Group[group].name);
}

QVariant MOption::option(const QString &key, const QString &group)
{
    QVariant v;

    /// add
    m_mutex.lock();
    m_settings->beginGroup(group);
    v = m_settings->value(key, defaultValue(key));
    m_settings->endGroup();
    m_mutex.unlock();

    return v;
}

QVariant MOption::option(const int &key, const int &group)
{
    return option(options[key].name, options_Group[group].name);
}
