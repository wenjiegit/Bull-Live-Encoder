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

#include "MThemeAdaptor.h"

#include <QVariant>
#include <QGuiApplication>
#include <QPixmap>
#include <QDebug>

MThemeAdaptor* MThemeAdaptor::m_this = 0;
MThemeAdaptor::MThemeAdaptor()
{
    m_currentSkinConfigPath = QGuiApplication::applicationDirPath() + "/theme/default/";
    m_currentSkinConfigName = "default.ini";
    m_settings = new QSettings(m_currentSkinConfigPath + m_currentSkinConfigName,
                               QSettings::IniFormat);
}

MThemeAdaptor *MThemeAdaptor::instance()
{
    if(!m_this) m_this = new MThemeAdaptor;

    return m_this;
}

void MThemeAdaptor::addColor(const QString& name, const QColor &c)
{
    QVariant v = c;
    m_settings->setValue(name, v);
}

QColor MThemeAdaptor::color(const QString& name)
{
    return m_settings->value(name).value<QColor>();
}

//void addPixmap(const QString &);
void MThemeAdaptor::addPixmap(const QString& name, const QString &p)
{
    m_settings->setValue(name, p);
}

QPixmap MThemeAdaptor::pixmap(const QString& name)
{
    QString pixPath = m_currentSkinConfigPath + m_settings->value(name).toString();
    return QPixmap(pixPath);
}

QString MThemeAdaptor::currentSkinConfig()
{
    return m_currentSkinConfigPath + m_currentSkinConfigName;
}
