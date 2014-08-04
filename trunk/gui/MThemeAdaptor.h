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

#ifndef MThemeAdaptor_H
#define MThemeAdaptor_H

#include "gui_global.h"

#include <QSettings>
#include <QColor>

class GUISHARED_EXPORT MThemeAdaptor {
public:
    void addColor(const QString& name, const QColor &c);
    QColor color(const QString& name);

    //void addPixmap(const QString &);
    void addPixmap(const QString& name, const QString &p);
    QPixmap pixmap(const QString& name);

    QString currentSkinConfig();

    static MThemeAdaptor *instance();

private:
    MThemeAdaptor();

private:
    QString m_currentSkinConfigName;
    QString m_currentSkinConfigPath;
    QSettings* m_settings;
    static MThemeAdaptor *m_this;
};

#endif // MThemeAdaptor_H
