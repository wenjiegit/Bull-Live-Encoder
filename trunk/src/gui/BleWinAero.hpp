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

#ifndef QTWIN_H
#define QTWIN_H

#include <QColor>
#include <QWidget>

class WindowNotifier;
class BleWin
{
public:
    static bool enableAeroWindow(QWidget *widget, bool enable = true);
    static bool enableAeroWindow_r(QWidget *widget, bool enable = true);
    static bool extendFrameIntoClientArea(QWidget *widget,
                                          int left = -1, int top = -1,
                                          int right = -1, int bottom = -1);
    static bool isCompositionEnabled();

    static bool isAeroEnabled();

    static QColor colorizatinColor();

    static void setWinAero(QWidget *window ,
                           int left = -1, int top = -1,
                           int right = -1, int bottom = -1);

    static void setAeroEnabled(bool enabled);

#ifdef Q_OS_WIN
    static bool EnableBlurBehindWindow(HWND window, bool enable = true, HRGN region = 0, bool transitionOnMaximized = false);
#endif

    static bool m_aeroEnabled;
private:
    static WindowNotifier *windowNotifier();
};

extern "C" {
bool resolveLibs();
}

#endif // QTWIN_H
