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

#include "BleWinAero.hpp"
#include <QLibrary>
#include <QApplication>
#include <QWidget>
#include <QList>
#include <QPointer>

#ifdef Q_OS_WIN
#include <qt_windows.h>
// Blur behind data structures
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified
#define WM_DWMCOMPOSITIONCHANGED        0x031E    // Composition changed window message
typedef struct _DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;
typedef struct _MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MARGINS, *PMARGINS;
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrDwmenableAeroWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);
typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind);

static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled= 0;
static PtrDwmenableAeroWindow pDwmenableAeroWindow = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;
static PtrDwmGetColorizationColor pDwmGetColorizationColor = 0;
static PtrDwmEnableBlurBehindWindow pDwmEnableBlurBehindWindow = 0;

class WindowNotifier : public QWidget
{
public:
    WindowNotifier() { winId(); }
    void addWidget(QWidget *widget) { widgets.append(widget); }
    void removeWidget(QWidget *widget) { widgets.removeAll(widget); }
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    QWidgetList widgets;
};
bool resolveLibs()
{
    if (!pDwmIsCompositionEnabled) {
        QLibrary dwmLib(QString::fromLocal8Bit("dwmapi"));
        pDwmIsCompositionEnabled =(PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
        pDwmenableAeroWindow = (PtrDwmenableAeroWindow)dwmLib.resolve("DwmenableAeroWindow");
        pDwmGetColorizationColor = (PtrDwmGetColorizationColor)dwmLib.resolve("DwmGetColorizationColor");

        pDwmEnableBlurBehindWindow = (PtrDwmEnableBlurBehindWindow)dwmLib.resolve("DwmEnableBlurBehindWindow");
    }
    return pDwmIsCompositionEnabled != 0;
}

#endif

bool BleWin::isCompositionEnabled()
{
#ifdef Q_OS_WIN
    if (resolveLibs()) {
        HRESULT hr = S_OK;
        BOOL isEnabled = false;
        hr = pDwmIsCompositionEnabled(&isEnabled);
        if (SUCCEEDED(hr))
            return isEnabled;
    }
#endif
    return false;
}

bool BleWin::isAeroEnabled()
{
    return m_aeroEnabled;
}

void BleWin::setAeroEnabled(bool enabled)
{
    m_aeroEnabled = enabled;
}

bool BleWin::m_aeroEnabled = false;

bool BleWin::enableAeroWindow(QWidget *widget, bool enable)
{
    Q_ASSERT(widget);
    bool result = false;
#ifdef Q_OS_WIN
    if (resolveLibs()) {
        DWM_BLURBEHIND bb;
        HRESULT hr = S_OK;
        bb.fEnable = enable;
        bb.dwFlags = DWM_BB_ENABLE;
        bb.hRgnBlur = NULL;
//        widget->setAttribute(Qt::WA_TranslucentBackground, enable);
//        widget->setAttribute(Qt::WA_NoSystemBackground, enable);
        hr = pDwmenableAeroWindow((HWND)widget->winId(), &bb);
        if (SUCCEEDED(hr)) {
            result = true;
            windowNotifier()->addWidget(widget);
        }
    }
#endif
    return result;
}

#ifdef Q_OS_WIN
bool BleWin::EnableBlurBehindWindow(HWND window,
                               bool enable,
                               HRGN region,
                               bool transitionOnMaximized)
{
    resolveLibs();

    DWM_BLURBEHIND blurBehind;

    blurBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED;
    blurBehind.fEnable = enable;
    blurBehind.fTransitionOnMaximized = transitionOnMaximized;

    if (enable && 0 != region)
    {
        blurBehind.dwFlags |= DWM_BB_BLURREGION;
        blurBehind.hRgnBlur = region;
    }

    if(pDwmEnableBlurBehindWindow)
        pDwmEnableBlurBehindWindow(window, &blurBehind);

     return true;
}
#endif

bool BleWin::enableAeroWindow_r(QWidget *widget, bool enable)
{
    Q_ASSERT(widget);
    bool result = false;
#ifdef Q_OS_WIN
    if (resolveLibs()) {
        DWM_BLURBEHIND bb;
        HRESULT hr = S_OK;
        bb.fEnable = enable;
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED;
        bb.hRgnBlur = NULL;
        bb.fTransitionOnMaximized = false;
        hr = pDwmenableAeroWindow((HWND)widget->winId(), &bb);
        if (SUCCEEDED(hr)) {
            result = true;
        }
    }
#endif
    return result;
}

bool BleWin::extendFrameIntoClientArea(QWidget *widget, int left, int top, int right, int bottom)
{
    Q_ASSERT(widget);
    Q_UNUSED(left);
    Q_UNUSED(top);
    Q_UNUSED(right);
    Q_UNUSED(bottom);
    bool result = false;
#ifdef Q_OS_WIN
    if (resolveLibs()) {
        QLibrary dwmLib(QString::fromLocal8Bit("dwmapi"));
        HRESULT hr = S_OK;
        MARGINS m = {left, top, right, bottom};
        hr = pDwmExtendFrameIntoClientArea((HWND)widget->winId(), &m);
        if (SUCCEEDED(hr)) {
            result = true;
            windowNotifier()->addWidget(widget);
        }
        widget->setAttribute(Qt::WA_TranslucentBackground, result);
    }
#endif
    return result;
}

QColor BleWin::colorizatinColor()
{
    QColor resultColor = QApplication::palette().window().color();
#ifdef Q_OS_WIN
    if (resolveLibs()) {
        DWORD color = 0;
        BOOL opaque = FALSE;
        QLibrary dwmLib(QString::fromLocal8Bit("dwmapi"));
        HRESULT hr = S_OK;
        hr = pDwmGetColorizationColor(&color, &opaque);
        if (SUCCEEDED(hr))
            resultColor = QColor(color);
    }
#endif
    return resultColor;
}

void BleWin::setWinAero(QWidget *window, int left, int top, int right, int bottom)
{
#ifdef Q_WS_X11
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setAttribute(Qt::WA_NoSystemBackground, false);
    QPalette pal = window->palette();
    QColor bg = pal.window().color();
    bg.setAlpha(180);
    pal.setColor(QPalette::Window, bg);
    window->setPalette(pal);
    window->ensurePolished(); // workaround Oxygen filling the background
    window->setAttribute(Qt::WA_StyledBackground, false);
#endif
    if (BleWin::isCompositionEnabled()) {
        BleWin::extendFrameIntoClientArea(window, left, top, right, bottom);
        window->setContentsMargins(0, 0, 0, 0);
    }
}

void setAeroEnabled(bool enabled)
{
    BleWin::m_aeroEnabled = enabled;
}

#ifdef Q_OS_WIN
WindowNotifier *BleWin::windowNotifier()
{
    static WindowNotifier *windowNotifierInstance = 0;
    if (!windowNotifierInstance)
        windowNotifierInstance = new WindowNotifier;
    return windowNotifierInstance;
}

bool WindowNotifier::nativeEvent(const QByteArray & eventType, void * _message, long * result)
{
    MSG *message = (MSG*)_message;
    if (message && message->message == WM_DWMCOMPOSITIONCHANGED) {
        bool compositionEnabled = BleWin::isCompositionEnabled();
        foreach(QWidget * widget, widgets) {
            if (widget) {
                widget->setAttribute(Qt::WA_NoSystemBackground, compositionEnabled);
            }
            widget->update();
        }
    }

    return QWidget::nativeEvent(eventType, message, result);
}

#endif
