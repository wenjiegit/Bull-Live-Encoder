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
#include "BleWinAero.hpp"
#include "BleAudioCapture.hpp"
#include "BleErrno.hpp"
#include "QjtMessageBox.h"

#define BLE_TITLE "Bull Live Encoder"

BleMainWindow::BleMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ThemedWidgetBase(this),
    ui(new Ui::BleMainWindow)
  , m_maxBtn(false)
  , m_skinWidget(0)
{
    ui->setupUi(this);

    // set image
    ui->addCameraBtn->setPixmap(QPixmap(":/image/add_camera.png"));
    ui->addWindowGrabBtn->setPixmap(QPixmap(":/image/add_grab.png"));
    ui->addPicBtn->setPixmap(QPixmap(":/image/add_pic.png"));
    ui->startBtn->setPixmap(QPixmap(":/image/ble_start.png"));
    ui->stopBtn->setPixmap(QPixmap(":/image/ble_stop.png"));

    ui->addCameraBtn->setFixedHeight(48);
    ui->addWindowGrabBtn->setFixedHeight(48);
    ui->addPicBtn->setFixedHeight(48);
    ui->startBtn->setFixedHeight(48);
    ui->stopBtn->setFixedHeight(48);

    ui->addCameraBtn->setToolTip(tr("add a camera source"));
    ui->addWindowGrabBtn->setToolTip(tr("add a desktop grab source"));
    ui->addPicBtn->setToolTip(tr("add a picture source"));

    ui->titleWidget->setFixedHeight(32);
    ui->titleWidget->setTitle(BLE_TITLE);
    ui->titleWidget->setHints(TitleWidget::MAX_HINT | TitleWidget::Min_HINT
                              | TitleWidget::CLOSE_HINT | TitleWidget::SKIN_HINT);

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

    // tray setting
    m_systemTrayIcon = new QSystemTrayIcon(QIcon(":/image/logo.png"), this);
    connect(m_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
    m_systemTrayIcon->show();

    BleImageDisplayArea *displayArea = new BleImageDisplayArea(this);
    m_imageProcessWidget = new BleImageProcess(this);
    displayArea->addWidget(m_imageProcessWidget);
    addTabWidget(displayArea, QPixmap(":/image/ble_live.png"), tr("live"));

    BleSetting *settings = new BleSetting(this);
    addTabWidget(settings, QPixmap(":/image/ble_setting.png"), tr("Settings"));

    m_encoderThread = new BleEncoderThread(this);
    m_sendThread = new BleRtmpSendThread(this);
    m_sendThread->setEncodeThread(m_encoderThread);

    m_imageProcessThread = new BleImageProcessThread(this);

    m_imageProcessWidget->setProcessThread(m_imageProcessThread);
    m_encoderThread->setProcessThread(m_imageProcessThread);

#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(this);
#endif

    setWindowTitle(BLE_TITLE);
}

BleMainWindow::~BleMainWindow()
{
    STOP_THREAD(m_sendThread);
    STOP_THREAD(m_encoderThread);
    STOP_THREAD(m_imageProcessThread);

    delete ui;
}

void BleMainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    m_titleHeight = ui->titleWidget->height() + ui->TabBar->height();
    m_linearHeight = 10;
    m_statusHeight = ui->statusBar->height();

    ThemedWidgetBase::drawThemedStyle(p);
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
    if(!m_maxBtn)
    {
        m_rectRestoreWindow = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        framelessHelper->setWidgetMovable(false);
        framelessHelper->setWidgetResizable(false);
        ui->titleWidget->button(TitleWidget::MAX_HINT)->setToolTip(tr("restore"));
    }
    else
    {
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
    menu.addAction(tr("&quit"), this, SLOT(onClose()));

    QAbstractButton *menuButton = ui->titleWidget->button(TitleWidget::MENU_HINT);
    menu.exec(menuButton->mapToGlobal(menu.rect().bottomLeft()));
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
        menu.addAction(tr("show"), this, SLOT(show()));
        menu.addAction(tr("hide"), this, SLOT(hide()));
        menu.addAction(tr("quit"), this, SLOT(onClose()));

        QPoint globalPoint = QCursor::pos();
        menu.exec(globalPoint);
    }
        break;
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
    default:
    {
        show();
        raise();
    }
        break;
    }
}

void BleMainWindow::onEncodeStart()
{
    m_encoderThread->init();

    // TODO make this a option
    QSize si = MOption::instance()->option("res", "encoder").toSize();
    int fps = MOption::instance()->option("fps", "encoder").toInt();

    m_imageProcessThread->setResolution(si.width(), si.height());
    m_imageProcessThread->setInternal(1000 / fps);

    START_THREAD(m_encoderThread);
    START_THREAD(m_sendThread);
    START_THREAD(m_imageProcessThread);

    m_audioCaptureThread = new BleAudioCapture;

    int audioSampleRate = MOption::instance()->option("sample_rate", "audio").toInt();

    int audioChannels = -1;
    QString audioChannelsStr = MOption::instance()->option("channels", "audio").toString();
    if (audioChannelsStr == "Mono") {
        audioChannels = 1;
    } else if (audioChannelsStr == "Stereo") {
        audioChannels = 2;
    }

    int audioBitrate = MOption::instance()->option("bitrate", "audio").toInt() * 1000;
    int devID = MOption::instance()->option("dev_id", "audio").toInt();

    int ret = m_audioCaptureThread->startCapture(audioBitrate, audioSampleRate, audioChannels, devID);
    if (ret != BLE_SUCESS) {
        QjtMessageBox::critical(this, tr("error"), tr("start audio capture error."));
    }

    ui->startBtn->setEnabled(false);
}

void BleMainWindow::onEncodeStop()
{
    STOP_THREAD(m_sendThread);
    STOP_THREAD(m_encoderThread);
    STOP_THREAD(m_imageProcessThread);
    STOP_THREAD(m_audioCaptureThread);

    m_audioCaptureThread->stopCapture();

    BleFree(m_audioCaptureThread);

    ui->startBtn->setEnabled(true);
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

    m_imageProcessWidget->addCaptureSource(source, 0, 0, 640, 480);
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

    m_imageProcessWidget->addCaptureSource(source, 0, 0, rect.width(), rect.height());
}

void BleMainWindow::onAddPic()
{
    QString picName = QFileDialog::getOpenFileName(this, tr("please select a picture"),
                                                    "",
                                                    tr("Images (*.png *.bmp *.jpg)"));

    if (picName.isEmpty()) return;

    BlePictureSource *source = new BlePictureSource(picName);
    m_imageProcessWidget->addCaptureSource(source, 10, 10, 320, 240);
}
