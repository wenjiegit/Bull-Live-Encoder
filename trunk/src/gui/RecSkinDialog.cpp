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

#include "RecSkinDialog.h"
#include "ui_RecSkinDialog.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QtGui>
#include <QMenu>
#include <QMessageBox>
#include <QDesktopWidget>

#include "MOption.h"
#include "mutility.h"
#include "MThemeWidgetBase.h"

RecSkinDialog::RecSkinDialog(const QImage &mainImage, const QImage &skinImage, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RecSkinDialog)
    , red(0)
    , green(0)
    , blue(0)
    , increase(10)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    SkinSimulationWidget = new QKSkinSimulationWidget(this);
    connect(SkinSimulationWidget,SIGNAL(sigSave()),this,SLOT(onSave()));
    connect(SkinSimulationWidget,SIGNAL(sigCancel()),this,SLOT(onCancel()));
    setImage(mainImage,skinImage);
}

void RecSkinDialog::setImage(const QImage &mainImage, const QImage &skinImage)
{
    this->skinImage = skinImage;

    int num = 0;
    for(int i = 0; i < skinImage.width(); ++i)
    {
        for(int j = 0; j < skinImage.height(); ++j)
        {
            ++num;
            QRgb rgbValue(skinImage.pixel(i, j));
            red += qRed(rgbValue);
            green += qGreen(rgbValue);
            blue += qBlue(rgbValue);
        }
    }

    red = red/num;
    green = green/num;
    blue = blue/num;
    SkinSimulationWidget->setImage(mainImage);
}

void RecSkinDialog::onSave()
{
    int x = SkinSimulationWidget->pos().x();
    int y = SkinSimulationWidget->pos().y();
    SkinSimulationWidget->hide();
    this->hide();
    QPixmap savePix(QPixmap::grabWidget(this,x,y,
                                        this->width() - x,
                                        this->height() - y));
    SkinSimulationWidget->show();

    QString basePath = "/theme/saved/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".png";
    QString fileName(QApplication::applicationDirPath() + basePath);

    if(!savePix.save(fileName, "PNG"))
        QMessageBox::warning(0,tr("error"),tr("save pixmap error"));
    else {
        MOption::instance()->setOption(basePath, OPTION_WindowBGPixmap, OPTION_GROUP_Theme);
        MOption::instance()->setOption("bitmap", OPTION_WindowBGPixmapType, OPTION_GROUP_Theme);
        updateThemedWidgets();
    }

    emit sigSavePix(basePath, savePix, QColor(red, green, blue));
}

void RecSkinDialog::onCancel()
{
    hide();
}

void RecSkinDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    if(!skinImage.isNull())
    {
        QPixmap image(QPixmap::fromImage(skinImage));
        painter.drawPixmap(0,0,image);

        QRect imageRect(0,0,image.width(),image.height());
        QColor imageColor(red,green,blue,100);
        painter.setPen(Qt::NoPen);
        painter.setBrush(imageColor);
        painter.drawRect(imageRect);

        QColor imageLeftColor(red,green,blue,5);
        QColor imageLeftColorr2(red,green,blue,20);
        QColor imageLeftColorr3(red,green,blue,100);
        QColor imageLeftColorr4(red,green,blue,255);
        QColor imageLeftColorr5(red,green,blue,255);
        QLinearGradient leftKnobGradient(image.width()/4*3,0,image.width() + increase,0);
        leftKnobGradient.setColorAt(0.0, imageLeftColor);
        leftKnobGradient.setColorAt(0.1, imageLeftColorr2);
        leftKnobGradient.setColorAt(0.2, imageLeftColorr3);
        leftKnobGradient.setColorAt(0.8, imageLeftColorr4);
        leftKnobGradient.setColorAt(1.0, imageLeftColorr5);
        QRect imageLeftRect(image.width()/4*3,0,image.width()/4 + increase,image.height());
        painter.setPen(Qt::NoPen);
        painter.setBrush(leftKnobGradient);
        painter.drawRect(imageLeftRect);


        QLinearGradient bottonKnobGradient(0,image.height()/4*3,0,image.height() + increase);
        bottonKnobGradient.setColorAt(0.0, imageLeftColor);
        bottonKnobGradient.setColorAt(0.1, imageLeftColorr2);
        bottonKnobGradient.setColorAt(0.2, imageLeftColorr3);
        bottonKnobGradient.setColorAt(0.8, imageLeftColorr4);
        bottonKnobGradient.setColorAt(1.0, imageLeftColorr5);
        QRect imageBottonRect(0,image.height()/4*3,image.width()*2 ,image.height()/4 + increase);
        painter.setPen(Qt::NoPen);
        painter.setBrush(bottonKnobGradient);
        painter.drawRect(imageBottonRect);


        QRect leftRect(image.width(),0,this->width() - image.width(),image.height());
        QColor leftColor(red,green,blue);
        painter.setPen(leftColor);
        painter.setBrush(leftColor);
        painter.drawRect(leftRect);




        QRect bottonRect(0,image.height(),this->width(),this->height() - image.height());
        painter.drawRect(bottonRect);
    }
}

RecSkinDialog::~RecSkinDialog()
{
    delete ui;
}

QKSkinSimulationWidget::QKSkinSimulationWidget(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    menu = new QMenu(this);
    QAction *save = menu->addAction(tr("����"));
    connect(save,SIGNAL(triggered()),this,SIGNAL(sigSave()));

    QAction *cancel = menu->addAction(tr("ȡ��"));
    connect(cancel,SIGNAL(triggered()),this,SIGNAL(sigCancel()));

}

void QKSkinSimulationWidget::setImage(const QImage &image)
{
    int w = image.width();
    int h = image.height();
    QImage temp(QSize(w, h), QImage::Format_ARGB32);
    for(int ww = 0; ww < w; ++ww)
    {
        for(int hh = 0; hh < h; ++hh)
        {
            QRgb rgb = image.pixel(ww, hh);
            QRgb r = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 150);
            temp.setPixel(ww, hh, r);
        }
    }
    this->image = temp;
    setFixedSize(image.width(),image.height());
    update();
}

void QKSkinSimulationWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    if(!image.isNull())
    {
        painter.drawPixmap(0,0,QPixmap::fromImage(image));

        //        QRect imageRect(0,0,image.width(),image.height());
        //        painter.setPen(Qt::red);
        //        painter.drawRect(imageRect);
    }
}

void QKSkinSimulationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    if(event->button() == Qt::RightButton)
        menu->exec(cursor().pos());
}

void QKSkinSimulationWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void QKSkinSimulationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //QSize size = QApplication::desktop()->size();
    QPoint g_topPoint = geometry().topLeft();
    g_topPoint = parentWidget()->mapToGlobal(g_topPoint);
    if(g_topPoint.y() < 0)
        move(event->globalPos().x() - dragPosition.x(),0);

    if(g_topPoint.x() < 0)
        move(0,event->globalPos().y() - dragPosition.y());

    if(g_topPoint.y() < 0 && g_topPoint.x() < 0)
        move(0,0);
}

void QKSkinSimulationWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit sigSave();
}
