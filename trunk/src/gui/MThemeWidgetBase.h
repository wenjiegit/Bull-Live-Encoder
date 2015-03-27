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

#ifndef MTHEMEWIDGETBASE_H
#define MTHEMEWIDGETBASE_H

#include <QPixmap>
#include <QColor>

class QWidget;

class ThemedWidgetBase
{
public:
    ThemedWidgetBase(QWidget* parent);
    virtual ~ThemedWidgetBase();

    virtual void applyNewTheme();
    void drawThemedStyle(QPainter &p, bool drawLinar = true);

protected:
    int m_titleHeight;
    int m_linearHeight;
    int m_statusHeight;

private:
    QWidget *m_themedWidget;
    QPixmap m_cachedPixmap;
    QColor m_cachedColor;
    int m_aeroTransparent;
    int m_widgetTransparent;
};

class WidgetColorAdapter
{
public:
    WidgetColorAdapter();
    virtual ~WidgetColorAdapter();
    virtual void applyNewAdapter();
};


extern "C"
{
void regThemedWidget(ThemedWidgetBase *w);
void removeThemedWidget(ThemedWidgetBase *w);
void updateThemedWidgets();

void updateTheme();

void regColorAdapterWidget(WidgetColorAdapter *w);
void removeColorAdapterWidget(WidgetColorAdapter *w);

// find the suitable font color, adapt to current theme.
QColor adapterColor();
}

#endif // MTHEMEWIDGETBASE_H
