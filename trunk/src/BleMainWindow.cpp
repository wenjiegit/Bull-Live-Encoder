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

#include "BleMainWindow.hpp"
#include "ui_BleMainWindow.h"

#include <QMenu>
#include <QSystemTrayIcon>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QtWin>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QLabel>

#include "NcFramelessHelper.h"
#include "MThemeAdaptor.h"
#include "MOption.h"
#include "MSkinChanger.h"

#include "BleSetting.hpp"
#include "BleEncoderThread.hpp"
#include "BleRtmpSendThread.hpp"
#include "BleUtil.hpp"

#include "BleCameraSource.hpp"
#include "BleWindowsCaptureSource.hpp"
#include "BleImageProcessThread.hpp"
#include "BleImageProcess.hpp"
#include "BleImageDisplayArea.hpp"
#include "BleCameraSelector.hpp"
#include "BleDesktopAreaSelector.hpp"
#include "BlePictureSource.hpp"
#include "BleAudioCapture.hpp"
#include "BleFileSource.hpp"
#include "BleTextSource.hpp"
#include "BleErrno.hpp"
#include "QjtMessageBox.h"
#include "BleVersion.hpp"
#include "BleAVQueue.hpp"
#include "BleImageCaptureThread.hpp"
#include "BleAVContext.hpp"
#include "BleContext.hpp"
#include "BleMediaSelector.hpp"
#include "BleSceneWidget.hpp"
#include "BleNetStreamSource.hpp"
#include "BleSceneWidget.hpp"

#define BLE_TITLE "Bull Live Encoder"

BleMainWindow::BleMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ThemedWidgetBase(this),
    ui(new Ui::BleMainWindow)
  , m_maxBtn(false)
  , m_skinWidget(0)
  , m_encoderThread(NULL)
  , m_sendThread(NULL)
  , m_imageProcessThread(NULL)
  , m_audioCaptureThread(NULL)
  , m_sceneWidget(NULL)
{
    ui->setupUi(this);

    // set image
    ui->addCameraBtn->setPixmap(QPixmap(":/image/add_camera.png"));
    ui->addWindowGrabBtn->setPixmap(QPixmap(":/image/add_grab.png"));
    ui->addPicBtn->setPixmap(QPixmap(":/image/add_pic.png"));
    ui->startBtn->setPixmap(QPixmap(":/image/ble_start.png"));
    ui->stopBtn->setPixmap(QPixmap(":/image/ble_stop.png"));
    ui->addFileSourceBtn->setPixmap(QPixmap(":/image/add_file.png"));
    ui->add_media->setPixmap(QPixmap(":/image/ble_media.png"));

    ui->addCameraBtn->setFixedHeight(48);
    ui->addWindowGrabBtn->setFixedHeight(48);
    ui->addPicBtn->setFixedHeight(48);
    ui->startBtn->setFixedHeight(48);
    ui->stopBtn->setFixedHeight(48);
    ui->addFileSourceBtn->setFixedHeight(48);
    ui->add_media->setFixedHeight(48);
    ui->addTextBtn->setFixedHeight(48);

    ui->addCameraBtn->setToolTip(tr("add a camera source"));
    ui->addWindowGrabBtn->setToolTip(tr("add a desktop grab source"));
    ui->addPicBtn->setToolTip(tr("add a picture source"));
    ui->addFileSourceBtn->setToolTip(tr("add a file source"));
    ui->addTextBtn->setToolTip(tr("add text source"));
    ui->add_media->setToolTip(tr("add media url"));
    ui->startBtn->setToolTip(tr("begin rtmp streaming"));
    ui->stopBtn->setToolTip(tr("stop rtmp streaming"));

    // TODO temp set false
    ui->addTextBtn->setVisible(false);

    ui->titleWidget->setFixedHeight(32);
    ui->titleWidget->setTitle(BLE_TITLE);
    ui->titleWidget->setHints(TitleWidget::Min_HINT
                              | TitleWidget::CLOSE_HINT | TitleWidget::SKIN_HINT | TitleWidget::MENU_HINT);

    framelessHelper = new NcFramelessHelper;
    framelessHelper->setWidgetMovable(true);
    framelessHelper->setWidgetResizable(true);
    framelessHelper->activateOn(this);

    connect(ui->titleWidget->button(TitleWidget::MAX_HINT),
            SIGNAL(clicked()), this, SLOT(onMax()));
    connect(ui->titleWidget->button(TitleWidget::Min_HINT),
            SIGNAL(clicked()), this, SLOT(onMin()));
    connect(ui->titleWidget->button(TitleWidget::CLOSE_HINT),
            SIGNAL(clicked()), this, SLOT(onClose()));
    connect(ui->titleWidget->button(TitleWidget::SKIN_HINT),
            SIGNAL(clicked()), this, SLOT(onSkin()));
    connect(ui->titleWidget->button(TitleWidget::MENU_HINT),
            SIGNAL(clicked()), this, SLOT(onMenu()));
    connect(ui->titleWidget, SIGNAL(doubleClicked()),
            this, SLOT(onDoubleClicked()));
    connect(ui->TabBar,SIGNAL(currentChanged(int)),
            this,SLOT(onTabBarCurrentChanged(int)));

    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(onEncodeStart()));
    connect(ui->stopBtn, SIGNAL(clicked()), this, SLOT(onEncodeStop()));

    connect(ui->addCameraBtn, SIGNAL(clicked())
            , this, SLOT(onAddCamera()));
    connect(ui->addWindowGrabBtn, SIGNAL(clicked())
            , this, SLOT(onAddWindowGrab()));
    connect(ui->addPicBtn, SIGNAL(clicked())
            , this, SLOT(onAddPic()));
    connect(ui->addFileSourceBtn, SIGNAL(clicked())
            , this, SLOT(onAddFileSource()));
    connect(ui->addTextBtn, SIGNAL(clicked())
            , this, SLOT(onAddTextSource()));
    connect(ui->add_media, SIGNAL(clicked())
            , this, SLOT(onAddMedia()));

    // tray setting
    m_systemTrayIcon = new QSystemTrayIcon(QIcon(":/image/logo.png"), this);
    connect(m_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
    m_systemTrayIcon->show();

    // m_imageProcessWidget = new BleImageProcess(this);
    // add scene widget
    m_sceneWidget = new BleSceneWidget(this);
    //m_sceneWidget->addWidget(m_imageProcessWidget);

    addTabWidget(m_sceneWidget, QPixmap(":/image/ble_live.png"), tr("live"));

    //BleImageDisplayArea *displayArea = new BleImageDisplayArea(this);

    //displayArea->addWidget(m_imageProcessWidget);

    BleSetting *settings = new BleSetting(this);
    addTabWidget(settings, QPixmap(":/image/ble_setting.png"), tr("Settings"));

    ui->TabBar->setVisible(false);

//    connect(settings, SIGNAL(settingChanged())
//            , m_imageProcessWidget, SLOT(onSettingChanged()));

#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(this);
#endif

    setWindowTitle(BLE_TITLE);
    QString versionStr = QString(tr("  version: %1")).arg(BLE_VERSION_STR);
    ui->statusBar->addWidget(new QLabel(versionStr));

    m_statusLabel = new QLabel(this);
    ui->statusBar->addWidget(m_statusLabel);
//    ui->TabBar->setVisible(false);

    settingChanged();
}

BleMainWindow::~BleMainWindow()
{
    onEncodeStop();

    delete ui;
}

void BleMainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    m_titleHeight = ui->titleWidget->height() + ui->TabBar->height() - 30;
    m_linearHeight = 20;
    m_statusHeight = ui->statusBar->height();

    ThemedWidgetBase::drawThemedStyle(p, false);
}

void BleMainWindow::showEvent(QShowEvent *e)
{
    if(isVisible()) repaint();
    QMainWindow::showEvent(e);
}

void BleMainWindow::addTabWidget(QWidget *widget, const QPixmap &pixmap, const QString &text)
{
    ui->stackedWidget->addWidget(widget);
    ui->TabBar->addTab(pixmap, text);
}

void BleMainWindow::settingChanged()
{
    QSize si = MOption::instance()->option("res", "encoder").toSize();

    if (si == QSize(320, 240)) {
        setFixedSize(862, 642);
    } else if (si == QSize(480, 360)) {
        setFixedSize(862, 642);
    } else if (si == QSize(640, 360)) {
        setFixedSize(1073, 642);
    } else if (si == QSize(640, 480)) {
        setFixedSize(862, 642);
    } else if (si == QSize(800, 600)) {
        setFixedSize(862, 642);
    } else if (si == QSize(1024, 768)) {
        setFixedSize(862, 642);
    } else if (si == QSize(1280, 720)) {
        setFixedSize(1073, 642);
    } else if (si == QSize(1440, 900)) {
        setFixedSize(987, 642);
    } else if (si == QSize(1600, 900)) {
        setFixedSize(1073, 642);
    } else if (si == QSize(1920, 1080)) {
        setFixedSize(1073, 642);
    }
}

void BleMainWindow::onMin()
{
    int minimumToTrayState = MOption::instance()->option(OPTION_MINTOTRAY, OPTION_GROUP_Main).toInt();
    if(minimumToTrayState == Qt::Checked)
    {
        QTimer::singleShot(0, this, SLOT(hide()));
        return;
    }

    showMinimized();
}

void BleMainWindow::onMax()
{
    if(!m_maxBtn) {
        m_rectRestoreWindow = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        framelessHelper->setWidgetMovable(false);
        framelessHelper->setWidgetResizable(false);
        ui->titleWidget->button(TitleWidget::MAX_HINT)->setToolTip(tr("restore"));
    } else {
        setGeometry(m_rectRestoreWindow);
        framelessHelper->setWidgetMovable(true);
        framelessHelper->setWidgetResizable(true);
        ui->titleWidget->button(TitleWidget::MAX_HINT)->setToolTip(tr("maximum"));
    }

    m_maxBtn = !m_maxBtn;
}

void BleMainWindow::onClose()
{
    qApp->quit();
}

void BleMainWindow::onSettins()
{
    BleSettingDialog dialog;
    dialog.exec();
}

void BleMainWindow::onDoubleClicked()
{
    onMax();
}

void BleMainWindow::onSkin()
{
    if(!m_skinWidget)
        m_skinWidget = new MSkinChangerWidget;

    if(!m_skinWidget->isVisible())
        m_skinWidget->show();

    m_skinWidget->raise();
}

void BleMainWindow::onMenu()
{
    QMenu menu(this);
    menu.addAction(tr("&Quit"), this, SLOT(onClose()));
    menu.addAction(tr("&Settings"), this, SLOT(onSettins()));

    QAbstractButton *menuButton = ui->titleWidget->button(TitleWidget::MENU_HINT);
    menu.exec(menuButton->mapToGlobal(menu.rect().bottomLeft()));
}

void BleMainWindow::onShow()
{
    showNormal();
    raise();
}

void BleMainWindow::onTabBarCurrentChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void BleMainWindow::activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Context:
    {
        QMenu menu;
        menu.addAction(tr("show"), this, SLOT(onShow()));
        menu.addAction(tr("hide"), this, SLOT(hide()));
        menu.addAction(tr("quit"), this, SLOT(onClose()));

        QPoint globalPoint = QCursor::pos();
        menu.exec(globalPoint);

        break;
    }
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
    default:
        if (!isVisible()) {
            onShow();
        } else {
            hide();
        }
        break;
    }
}

void BleMainWindow::onEncodeStart()
{
//    m_imageProcessThread = new BleImageProcessThread(this);
//    m_imageProcessWidget->setProcessThread(m_imageProcessThread);

//    QSize si = MOption::instance()->option("res", "encoder").toSize();
//    int fps = MOption::instance()->option("fps", "encoder").toInt();
//    m_imageProcessThread->setResolution(si.width(), si.height());
//    m_imageProcessThread->setInternal(1000 / fps);

    m_encoderThread = new BleEncoderThread(this);
    m_encoderThread->init();

    m_audioCaptureThread = new BleAudioCapture;

    int audioSampleRate = MOption::instance()->option("sample_rate", "audio").toInt();

    int audioChannels = -1;
    QString audioChannelsStr = MOption::instance()->option("channels", "audio").toString();
    if (audioChannelsStr == "Mono") {
        audioChannels = 1;
    } else if (audioChannelsStr == "Stereo") {
        audioChannels = 2;
    }
    BleAssert(audioChannels != -1);

    int audioBitrate = MOption::instance()->option("bitrate", "audio").toInt() * 1000;
    int devID = MOption::instance()->option("dev_id", "audio").toInt();

    int ret = m_audioCaptureThread->startCapture(audioBitrate, audioSampleRate, audioChannels, devID);
    if (ret != BLE_SUCESS) {
        QjtMessageBox::critical(this, tr("error"), tr("start audio capture error."));
    }

    if (!m_sendThread) {
        m_sendThread = new BleRtmpSendThread(this);
        connect(m_sendThread, SIGNAL(status(int,int,int,qint64))
                , this, SLOT(onStatus(int,int,int,qint64)));

        START_THREAD(m_sendThread);
    }

    BleImageCaptureThread *captureThread = new BleImageCaptureThread;
    BleAVContext::instance()->captureThread = captureThread;
//    captureThread->setImageProcessThread(m_imageProcessThread);

    START_THREAD(m_encoderThread);
    //START_THREAD(m_imageProcessThread);
    START_THREAD(m_audioCaptureThread);
    START_THREAD(captureThread);

    ui->startBtn->setEnabled(false);
    ui->startBtn->setButtonStatus(STATUS_DISABLED);
}

void BleMainWindow::onEncodeStop()
{
    SAFE_STOP_THREAD(BleAVContext::instance()->captureThread);
    SAFE_STOP_THREAD(m_sendThread);
    SAFE_STOP_THREAD(m_encoderThread);
    //SAFE_STOP_THREAD(m_imageProcessThread);
    SAFE_STOP_THREAD(m_audioCaptureThread);

    if (m_audioCaptureThread) {
        m_audioCaptureThread->stopCapture();
    }

    if (m_encoderThread) {
        m_encoderThread->fini();
    }

    if (BleAVContext::instance()->captureThread) {
        BleAVContext::instance()->captureThread->fini();
    }

    BleFree(m_sendThread);
    BleFree(m_encoderThread);
    //BleFree(m_imageProcessThread);
    BleFree(m_audioCaptureThread);
    BleFree(BleAVContext::instance()->captureThread);

    //m_imageProcessWidget->setProcessThread(m_imageProcessThread);

    BleAVQueue::destroy();
    appCtx->fini();

    ui->startBtn->setEnabled(true);
    ui->startBtn->setButtonStatus(STATUS_NORMAL);
}

void BleMainWindow::onAddCamera()
{
    BleCameraSelectorDialog dialog;
    int res = dialog.exec();
    if (res != BleCameraSelectorDialog::Accepted) {
        return;
    }

    int fps = MOption::instance()->option("fps", "encoder").toInt();

    BleCameraSource *source = new BleCameraSource(this);
    source->setCameraInfo(dialog.selectedCameraIndex(), dialog.selectedCameraName());
    source->setCaptureInterval(1000 / fps / 1.5);
    source->start();

    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onAddWindowGrab()
{
    BleDesktopAreaSelector *areaSelector = new BleDesktopAreaSelector;
    connect(areaSelector, SIGNAL(areaSelected(QRect))
            , this, SLOT(onDesktopSelected(QRect)));
    areaSelector->showFullScreen();
}

void BleMainWindow::onDesktopSelected(const QRect &rect)
{
    int fps = MOption::instance()->option("fps", "encoder").toInt();

    BleWindowsCaptureSource *source = new BleWindowsCaptureSource(this);
    source->setGrabInfo(0, rect.x(), rect.y(), rect.width(), rect.height());
    source->setCaptureInterval(1000 / fps);
    source->start();

    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onAddPic()
{
    QString picName = QFileDialog::getOpenFileName(this, tr("please select a picture"),
                                                    "",
                                                    tr("Images (*.png *.bmp *.jpg *.gif)"));

    if (picName.isEmpty()) return;

    BlePictureSource *source = new BlePictureSource(picName);
    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onAddFileSource()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("please select a video file"),
                                                    "",
                                                    tr("Videos (*.flv *.rmvb *.mp4 *.mkv *.avi *.wmv);;"
                                                       "Audios (*.ape *.mp3 *.aac *.wma)"));

    if (fileName.isEmpty()) return;

    BleFileSource *source = new BleFileSource();
    source->setMedia(fileName);
    source->start();

    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onAddTextSource()
{
    QString text= "this is a text";
    BleTextSource *source = new BleTextSource;
    source->setText(text);

    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onAddMedia()
{
    BleMediaSelectorDialog dialog;
    int ret = dialog.exec();
    if (ret == QDialog::Rejected) return;

    QString addr = dialog.addr();
    if (addr.isEmpty()) {
        QjtMessageBox::critical(NULL, "", tr("url can not be empty."));
        return;
    }

    BleNetStreamSource *source = new BleNetStreamSource();
    source->setMedia(addr.trimmed());
    source->start();

    BleImageProcess *pwidget = BleSceneWidget::instance()->currentImageProcessWidget();
    pwidget->addCaptureSource(source, 0, 0, 640, 480);
}

void BleMainWindow::onStatus(int audioKbps, int videoKbps, int fps, qint64 sendDataCount)
{
    QString text = QString("  A: %1 kbps  V: %2 kbps  fps: %3  send: %4  ")
            .arg(audioKbps*8/1000).arg(videoKbps*8/1000).arg(fps).arg(formatS_size(sendDataCount));
    m_statusLabel->setText(text);
}
