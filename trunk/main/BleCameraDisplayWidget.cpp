#include "BleCameraDisplayWidget.h"
#include "ui_BleCameraDisplayWidget.h"

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QTimer>
#include <QDateTime>
#include <QAbstractVideoBuffer>
#include <QPainter>

#include <QDebug>

#include "MOption.h"
#include "BleErrno.h"
#include "BleUtil.h"

// opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

BleVideoSurface::BleVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
{

}

BleVideoSurface::~BleVideoSurface()
{

}

QList<QVideoFrame::PixelFormat> BleVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24;
}

bool BleVideoSurface::present(const QVideoFrame& frame)
{
    if (frame.isValid())
    {
        static int id = 0;

        QVideoFrame cloneFrame(frame);
        if (!cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) return false;
        emit imageAvailable(++id, cloneFrame);
        cloneFrame.unmap();
        return true;
    }

    return false;
}

BleCameraCapture::BleCameraCapture(QObject *parent)
    : BleThread(parent)
    , m_interval(40 * 1000)     // 40 ms
{

}

BleCameraCapture::~BleCameraCapture()
{

}

void BleCameraCapture::run()
{
    IplImage *pImg = NULL;
    CvCapture *cap = cvCaptureFromCAM(0);
    if (!cap) {
        log_error("open camera failed.");
        return;
    }

    while (!m_stop) {
        pImg = cvQueryFrame(cap);
        cvCvtColor(pImg, pImg, CV_BGR2RGB);
        emit imageAvailable(pImg->imageData, pImg->width, pImg->height);
        usleep(m_interval);
    }

    // TODO : ca not release cap or will crash.
    // cvReleaseCapture(&cap);

    log_trace("BleCameraCapture exit normally.");
}

void BleCameraCapture::setCaptureInterval(int interval)
{
    m_interval = interval;
}

BleCameraDisplayWidget::BleCameraDisplayWidget(QWidget *parent)
  : BleCaptureAbstract(parent)
  , ui(new Ui::BleCameraDisplayWidget)
  , m_camera(NULL)
  , m_viewfinder(NULL)
  , m_imageCapture(NULL)
  , m_cameraCaptureThread(NULL)
{
    ui->setupUi(this);

    connect(&m_captureTimer, SIGNAL(timeout())
            , this, SLOT(onCaptureTimeout()));

    m_videoSurface = new BleVideoSurface(this);
    connect(m_videoSurface, SIGNAL(imageAvailable(int,QVideoFrame))
            , this, SLOT(onImageAvailable(int,QVideoFrame)));

    m_cameraCaptureThread = new BleCameraCapture(this);
    connect(m_cameraCaptureThread, SIGNAL(imageAvailable(char*,int,int))
            , this, SLOT(onImageAvailable(char*,int,int)));
}

BleCameraDisplayWidget::~BleCameraDisplayWidget()
{
    delete ui;
}

int BleCameraDisplayWidget::startCapture()
{
    if (!m_cameraCaptureThread->isRunning()) {
        m_cameraCaptureThread->start();
    }

    return BLE_SUCESS;
}

int BleCameraDisplayWidget::stopCapture()
{
    m_captureTimer.stop();
    qDebug() << m_cameraCaptureThread->isRunning();
    if (m_cameraCaptureThread->isRunning()) {
        m_cameraCaptureThread->stop();
        m_cameraCaptureThread->wait();
    }

    BleFree(m_camera);
    BleFree(m_imageCapture);

    return BLE_SUCESS;
}

void BleCameraDisplayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    p.drawImage(0, 0, m_cachedPixmap);
}

void BleCameraDisplayWidget::onImageAvailable(int id, const QVideoFrame &buffer)
{
    Q_UNUSED(id);

    m_frames.append(buffer);
}

void BleCameraDisplayWidget::onImageAvailable(char *imageData, int w, int h)
{
    QImage preview((uchar*)imageData, w, h, QImage::Format_RGB888);

    m_cachedPixmap = preview;
    m_cachedPixmap.detach();
    this->update();

    emit imageGrabed(preview, QDateTime::currentDateTime());
}

void BleCameraDisplayWidget::onCaptureTimeout()
{
    if (m_frames.size() > 0) {
        QVideoFrame & buffer = m_frames.last();
        buffer.map(QAbstractVideoBuffer::ReadOnly);
        QImage preview(buffer.bits(), buffer.width(), buffer.height(), QVideoFrame::imageFormatFromPixelFormat(buffer.pixelFormat()));

        m_cachedPixmap = preview;
        m_cachedPixmap.detach();
        this->update();

        emit imageGrabed(preview, QDateTime::currentDateTime());
        buffer.unmap();
        m_frames.clear();
    }
}

