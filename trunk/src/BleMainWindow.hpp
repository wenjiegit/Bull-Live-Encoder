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

#ifndef BleMainWindow_H
#define BleMainWindow_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "MThemeWidgetBase.h"

namespace Ui {
    class BleMainWindow;
}

class NcFramelessHelper;
class MSkinChangerWidget;
class BleEncoderThread;
class BleCaptureAbstract;
class BleRtmpSendThread;
class BleCameraSource;
class BleImageProcessThread;
class BleImageProcess;
class BleAudioCapture;
class BleFileSource;
class BleSceneWidget;

class QLabel;

class BleMainWindow : public QMainWindow, public ThemedWidgetBase
{
    Q_OBJECT

public:
    explicit BleMainWindow(QWidget *parent = 0);
    ~BleMainWindow();

private:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);
    void addTabWidget(QWidget *widget, const QPixmap &pixmap, const QString &text);

private slots:
    void onMin();
    void onMax();
    void onClose();
    void onSettins();
    void onDoubleClicked();
    void onSkin();
    void onMenu();
    void onShow();
    void onTabBarCurrentChanged(int index);

    void activated(QSystemTrayIcon::ActivationReason reason);

    void onEncodeStart();
    void onEncodeStop();

    void onAddCamera();
    void onAddWindowGrab();
    void onDesktopSelected(const QRect &rect);
    void onAddPic();
    void onAddFileSource();
    void onAddTextSource();
    void onAddMedia();

    void onStatus(int audioKbps, int videoKbps, int fps, qint64 sendDataCount);
    void settingChanged();

private:
    Ui::BleMainWindow *ui;
    bool m_maxBtn;
    QRect m_rectRestoreWindow;
    QSystemTrayIcon *m_systemTrayIcon;

    NcFramelessHelper *framelessHelper;
    MSkinChangerWidget *m_skinWidget;
    BleEncoderThread *m_encoderThread;
    BleRtmpSendThread  *m_sendThread;
    BleImageProcessThread *m_imageProcessThread;
    BleImageProcess *m_imageProcessWidget;
    BleAudioCapture *m_audioCaptureThread;

    QLabel *m_statusLabel;
    BleSceneWidget *m_sceneWidget;
};

#endif // BleMainWindow_H
