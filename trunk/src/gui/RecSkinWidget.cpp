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

#include "RecSkinWidget.h"
#include "ui_RecSkinWidget.h"

#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QCoreApplication>
#include <QRect>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QFile>

#include "QjtMessageBox.h"
#include "MOption.h"

RecSkinWidget::RecSkinWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecSkinWidget)
    , nowPixNum(-1)
    , m_rowMaxPix(15)
{
    ui->setupUi(this);
    setFixedSize(490,255);

    QStringList l = MOption::instance()->option(OPTION_Recommand, OPTION_GROUP_Theme).toStringList();
    QString appPath = QCoreApplication::applicationDirPath() + "/";

    for(int i = 0; i < 4; ++i)
    {
        RecSkinPushButton *SkinPushButton = new RecSkinPushButton(this);
        connect(SkinPushButton,SIGNAL(currentPixmap(QString,QPixmap,QColor)),
                this,SIGNAL(currentPixmap(QString,QPixmap,QColor)));

        connect(SkinPushButton,SIGNAL(clickNum(int)),
                this,SLOT(onClickNum(int)));
        if(i <= l.size() - 1)
        {
            QString path = appPath + l.at(i);
            qDebug() << path;
            QPixmap pix(path);
            SkinPushButton->setToolTip("");
            if(i == 0)
            {
                QString themeType = MOption::instance()->option("WindowBGPixmapType", "theme").toString();


                QImage skinImage(pix.toImage());
                int num = 0;
                int red = 0;
                int green = 0;
                int blue = 0;
                for(int ii = 0; ii < skinImage.width(); ++ii)
                {
                    for(int j = 0; j < skinImage.height(); ++j)
                    {
                        ++num;
                        QRgb rgbValue(skinImage.pixel(ii, j));
                        red += qRed(rgbValue);
                        green += qGreen(rgbValue);
                        blue += qBlue(rgbValue);
                    }
                }
                if(num != 0)
                {
                    red = red/num;
                    green = green/num;
                    blue = blue/num;
                }

                oneFileName = path;
                onePix = pix;
                oneColor = QColor(red,green,blue,255);
                if(themeType.isEmpty())
                {
                    SkinPushButton->setPixmap(path,pix,oneColor,120,true);

                    QTimer::singleShot(1000, this, SLOT(getRgb()));
                }
                else
                {
                    SkinPushButton->setPixmap(path,pix,QColor(255,255,255,255),120);
                }
            }
            else
            {
                SkinPushButton->setPixmap(path,pix,QColor(255,255,255,255),120);
            }
            SkinPushButton->setSkin(true);
        }


        ui->horizontalLayoutDefault->addWidget(SkinPushButton);
    }

    QPixmap addPix(":/images/add.png");
    QPixmap historyPix(":/images/skinHistoryImage.png");
    RecSkinPushButton *button = new RecSkinPushButton(this);
    button->setPixmap("",addPix, QColor(255,255,255,255),60,false);
    connect(button,SIGNAL(clickResult()),
            this,SIGNAL(definePixmapResult()));
    ui->horizontalLayoutHistoryBotton->insertWidget(0,button);


    QString historyPath = QCoreApplication::applicationDirPath() + "/theme/saved";

    // TODO MEM leak
    QDir *dir = new QDir(historyPath);
    QStringList filter;

    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));

    QStringList fileNames;

    for(int i = 0; i < fileInfo->count(); ++ i)
    {
        if(fileInfo->at(i).isDir())
            continue;
        fileNames << fileInfo->at(i).fileName();
    }
    fileNames.sort();
    int fileSize = fileNames.size();
    if(fileSize > m_rowMaxPix)
        fileSize = m_rowMaxPix;

    for(int i = 0; i < m_rowMaxPix - fileSize; ++i)
    {
        addItem("",historyPix, QColor(255,255,255,255),false,false);
    }

    for(int i = 0; i < fileSize; ++i)
    {
        QString imagePathName = historyPath + "/" + fileNames.value(i);
        QPixmap pxi(imagePathName);
        addItem(imagePathName,pxi, QColor(255,255,255,255),false,true);
    }

    QString themeType = MOption::instance()->option("WindowBGPixmapType", "theme").toString();
    if(themeType == "bitmap")
    {
        QString fileName = MOption::instance()->option("WindowBGPixmap", "theme").toString();

        for(int i = 0; i < buttons.size(); ++i)
        {
            if(buttons.value(i)->getFileName() == fileName)
                nowPixNum = i;
        }
    }
}

void RecSkinWidget::getRgb()
{
    emit currentPixmap(oneFileName,onePix,oneColor);
}

void RecSkinWidget::onSavePix(const QString &fileName,const QPixmap &pix, const QColor &averageColor)
{
    addItem(fileName,pix,averageColor,true,true);
}

void RecSkinWidget::addItem(const QString &fileName,const QPixmap &pix,
                            const QColor &averageColor,bool isColor,bool isSkin)
{
    if(isSkin)
        emit currentPixmap(fileName,pix,averageColor);

    RecSkinPushButton *SkinPushButton = new RecSkinPushButton(this);
    connect(SkinPushButton,SIGNAL(currentPixmap(QString,QPixmap,QColor)),
            this,SIGNAL(currentPixmap(QString,QPixmap,QColor)));

    connect(SkinPushButton,SIGNAL(clickNum(int)),
            this,SLOT(onClickNum(int)));

    SkinPushButton->setPixmap(fileName,pix,averageColor,60,isColor);
    SkinPushButton->setSkin(isSkin);
    if(isSkin)
    {
        SkinPushButton->setCanDelete(true);

        nowPixNum = 0;
    }

    connect(SkinPushButton,SIGNAL(closeNum(int,QString)),
            this,SLOT(onCloseNum(int,QString)));

    ui->horizontalLayoutHistory->insertWidget(0,SkinPushButton);
    buttons.insert(0,SkinPushButton);
    int buttonNum = m_rowMaxPix/2 + 1;
    if(buttons.size() > buttonNum)
    {
        RecSkinPushButton *SkinPushButtonBotton = buttons.value(buttonNum);
        ui->horizontalLayoutHistory->removeWidget(SkinPushButtonBotton);
        ui->horizontalLayoutHistoryBotton->insertWidget(0,SkinPushButtonBotton);

        if(buttons.size() == m_rowMaxPix + 1)
        {
            int buttonbottonNum = m_rowMaxPix;
            ui->horizontalLayoutHistoryBotton->removeWidget(buttons.value(buttonbottonNum));
            RecSkinPushButton *delButton = buttons.takeAt(buttonbottonNum);
            QString fileName = delButton->getFileName();
            if(!fileName.isEmpty())
                QFile::remove(fileName);
            delButton->deleteLater();
        }
    }
    for(int i = 0; i < buttons.size(); ++i)
        buttons.value(i)->setNum(i);
}

void RecSkinWidget::onCloseNum(int num,const QString &fileName)
{
    int type = QjtMessageBox::warning(0,tr("warning"),tr("sure to delete ?"),QMessageBox::Ok|QMessageBox::No);
    if(type == QMessageBox::No)
    {
        return;
    }
    RecSkinPushButton *tmpButton = NULL;
    if(nowPixNum == num)
        emit currentPixmap(oneFileName,onePix,oneColor);
    else
        tmpButton = buttons.value(nowPixNum);
    if(num <= 7)
    {
        ui->horizontalLayoutHistory->removeWidget(buttons.value(num));
        buttons.takeAt(num)->deleteLater();
        int buttonNum = m_rowMaxPix/2;
        RecSkinPushButton *SkinPushButtonBotton = buttons.value(buttonNum);
        ui->horizontalLayoutHistoryBotton->removeWidget(SkinPushButtonBotton);
        ui->horizontalLayoutHistory->insertWidget(buttonNum,SkinPushButtonBotton);
    }
    else
    {
        ui->horizontalLayoutHistoryBotton->removeWidget(buttons.value(num));
        buttons.takeAt(num)->deleteLater();
    }
    QPixmap historyPix(":/images/skinHistoryImage.png");
    RecSkinPushButton *SkinPushButton = new RecSkinPushButton(this);
    connect(SkinPushButton,SIGNAL(currentPixmap(QString,QPixmap,QColor)),
            this,SIGNAL(currentPixmap(QString,QPixmap,QColor)));
    SkinPushButton->setPixmap("",historyPix,QColor(255,255,255,255),60,false);
    buttons << SkinPushButton;
    ui->horizontalLayoutHistoryBotton->insertWidget(6,SkinPushButton);

    for(int i = 0; i < buttons.size(); ++i)
    {
        buttons.value(i)->setNum(i);
        if(tmpButton == buttons.value(i))
            nowPixNum = i;
    }

    QFile::remove(fileName);
}

void RecSkinWidget::onClickNum(int num)
{
    nowPixNum = num;
}

RecSkinWidget::~RecSkinWidget()
{
    delete ui;
}

/******************************RecSkinPushButton******************/

RecSkinPushButton::RecSkinPushButton(QWidget *parent) :
    QPushButton(parent),
    isEnter(false)
  , isSkin(false)
  , canDelete(false),
    m_num(-1)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_closeButton = new QPushButton(this);
    QPixmap pixmap(":/images/close.png");
    m_closeButton->setIcon(QIcon(pixmap));
    m_closeButton->resize(pixmap.size());
    m_closeButton->setFlat(true);
    m_closeButton->setToolTip(tr("delete"));
    m_closeButton->hide();
    connect(m_closeButton,SIGNAL(clicked()),
            this,SLOT(onClose()));
}

void RecSkinPushButton::onClose()
{
    emit closeNum(m_num,fileName);
}

void RecSkinPushButton::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);

    if(isEnter)
    {
        if(isSkin)
        {
            painter.drawPixmap(0, 0, drawPixmap);
            //QRect blueRect(0, 0,width()-,this->height() - 2);
            QPen bluePen(QPen(QColor(0, 0, 255, 200)));
            bluePen.setWidth(3);
            painter.setPen(bluePen);
            painter.drawRect(rect());

            QRect whiteRect(0, 0, width()-4, height()-4);
            whiteRect.moveCenter(rect().center());
            painter.setPen(QPen(QColor(255, 255, 255, 150)));
            painter.drawRect(whiteRect);

            painter.fillRect(whiteRect, QBrush(QColor(0, 0, 0, 100)));

        }
        else
        {
            painter.drawPixmap(0,0,drawPixmap);
        }
    }
    else
    {
        painter.drawPixmap(0,0,drawPixmap);
    }
}

void RecSkinPushButton::enterEvent(QEvent */*e*/)
{
    isEnter = true;
    if(isSkin && canDelete)
        m_closeButton->show();
}

void RecSkinPushButton::leaveEvent(QEvent */*e*/)
{
    isEnter = false;
    m_closeButton->hide();
}

void RecSkinPushButton::mousePressEvent(QMouseEvent */*e*/)
{
    if(isSkin)
    {
        if(!isColorTrue)
        {
            QImage skinImage(originalPixmap.toImage());
            int num = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            for(int ii = 0; ii < skinImage.width(); ++ii)
            {
                for(int j = 0; j < skinImage.height(); ++j)
                {
                    ++num;
                    QRgb rgbValue(skinImage.pixel(ii, j));
                    red += qRed(rgbValue);
                    green += qGreen(rgbValue);
                    blue += qBlue(rgbValue);
                }
            }
            if(num != 0)
            {
                red = red/num;
                green = green/num;
                blue = blue/num;
            }
            averageColor = QColor(red,green,blue,255);
        }

        emit currentPixmap(fileName,originalPixmap,averageColor);
        emit clickNum(m_num);
    }
    else
    {
        emit clickResult();
    }

}

void RecSkinPushButton::resizeEvent(QResizeEvent *)
{

    QSize closeButtonSize = m_closeButton->iconSize();
    QRect closeButtonRect(rect().width()-closeButtonSize.width(),
                          0,
                          closeButtonSize.width(),
                          closeButtonSize.height());

    m_closeButton->setGeometry(closeButtonRect);
    m_closeButton->updateGeometry();
}

/********************************RecSkinColorPalette*********************/
RecSkinColorPalette::RecSkinColorPalette(QWidget *parent) :
    QWidget(parent),
    isMousePress(false),
    isDrawEllipse(false),
    hsvValue(0)
{
    setFixedSize(359,255);
    setWindowFlags(  Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    horizontalSliderValueChanged(255);
}

void RecSkinColorPalette::getColor(const QColor &averageColor)
{
    isDrawEllipse = true;
    emit sigMousePressRGB(averageColor.rgb());
    hsvValue = averageColor.value();
    int width = 359;
    int height = 255;
    QImage temp(QSize(width, height), QImage::Format_ARGB32);
    for(int w = 0; w < width; ++w)
    {
        for(int h = 0; h < height; ++h)
        {
            QRgb rgb = QColor::fromHsv(w,h,hsvValue,255).rgb();
            QRgb r = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 255);
            temp.setPixel(w, h, r);
        }
    }

    drawImage = temp;
    QImage tempImage = drawImage.scaled(this->width(),this->height());

    bool isTrue = false;
    for(int i = 0; i < tempImage.width(); ++i)
    {
        for(int j = 0; j < tempImage.height(); ++j)
        {
            if(QColor::fromRgb(tempImage.pixel(i,j)) == averageColor)
            {
                mousePoint.setX(i);
                mousePoint.setY(j);
                isTrue = true;
                break;
            }

        }
        if(isTrue)
            break;
    }
    update();
}

void RecSkinColorPalette::horizontalSliderValueChanged(int value)
{
    hsvValue = value;
    int width = 359;
    int height = 255;
    QImage temp(QSize(width, height), QImage::Format_ARGB32);
    for(int w = 0; w < width; ++w)
    {
        for(int h = 0; h < height; ++h)
        {
            QRgb rgb = QColor::fromHsv(w,h,value,255).rgb();
            QRgb r = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 255);
            temp.setPixel(w, h, r);
        }
    }
    drawImage = temp;
    update();
}

void RecSkinColorPalette::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(!drawImage.isNull())
        painter.drawPixmap(rect(), QPixmap::fromImage(drawImage));
    if(isDrawEllipse)
    {
        int r = 8;
        //�滭СԲ
        QColor color;
        if(hsvValue < 255/2)
            color = QColor(255,255,255,255);
        else
            color = QColor(0,0,0,255);
        painter.setPen(color);
        painter.setBrush(QBrush(color,Qt::SolidPattern));
        painter.drawEllipse(QRectF(mousePoint.x() - r/2,mousePoint.y() - r/2,r,r));
    }
}

void RecSkinColorPalette::mousePressEvent(QMouseEvent *e)
{
    isDrawEllipse = true;
    if(e->x() > 0
            && e->x() < this->width()
            && e->y() > 0
            && e->y() < this->height())
    {
        QRgb rgb = drawImage.scaled(this->width(),this->height()).pixel(e->x(),e->y());
        emit sigMousePressRGB(rgb);
        emit currentColor(QColor::fromRgb(rgb));
        mousePoint = e->pos();
        update();
    }
    isMousePress = true;
}

void RecSkinColorPalette::mouseReleaseEvent(QMouseEvent *)
{
    isMousePress = false;
}

void RecSkinColorPalette::mouseMoveEvent(QMouseEvent *e)
{
    if(isMousePress)
    {
        if(e->x() > 0
                && e->x() < this->width()
                && e->y() > 0
                && e->y() < this->height())
        {
            QRgb rgb = drawImage.scaled(this->width(),this->height()).pixel(e->x(),e->y());
            emit sigMousePressRGB(rgb);
            emit currentColor(QColor::fromRgb(rgb));
            mousePoint = e->pos();
            update();
        }
    }
}

/****************************RecSkinColorBrightnessRange******************************/
RecSkinColorBrightnessRange::RecSkinColorBrightnessRange(QWidget *parent) :
    QWidget(parent),
    isMousePress(false),
    isDrawEllipse(false)
{
    setFixedSize(15,255);
    setAttribute(Qt::WA_TranslucentBackground);
}

void RecSkinColorBrightnessRange::onGetColor(const QRgb &rgb)
{
    isDrawEllipse = true;
    mousePoint.setX(this->width()/2);
    mousePoint.setY(qAbs(QColor::fromRgb(rgb).value() - 255));
    int width = this->width();
    int height = this->height();
    QImage temp(QSize(width, height), QImage::Format_ARGB32);
    QColor color(QColor::fromRgb(rgb));
    int hue = color.hsvHue();
    int Saturation = color.hsvSaturation();
    for(int h = 0; h < height; ++h)
    {
        for(int w = 0; w < width; ++w)
        {
            QRgb rgbs = QColor::fromHsv(hue,Saturation,qAbs(h - 255),255).rgb();
            temp.setPixel(w, h, rgbs);
        }
    }
    drawImage = temp;
    update();
}

void RecSkinColorBrightnessRange::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(!drawImage.isNull())
        painter.drawPixmap(rect(), QPixmap::fromImage(drawImage));
    if(isDrawEllipse)
    {
        int r = 8;
        //�滭СԲ
        QColor color;
        if(mousePoint.y() > 255/2)
            color = QColor(255,255,255,255);
        else
            color = QColor(0,0,0,255);
        painter.setPen(color);
        painter.setBrush(QBrush(color,Qt::SolidPattern));
        painter.drawEllipse(QRectF(this->width()/2 - r/2,mousePoint.y() - r/2,r,r));
    }
}

void RecSkinColorBrightnessRange::mousePressEvent(QMouseEvent *e)
{
    isDrawEllipse = true;
    if(e->x() > 0 && e->x() < this->width() && e->y() < 255 && e->y() > 0)
    {
        QRgb rgb = drawImage.scaled(this->width(),this->height()).pixel(e->x(),e->y());
        emit currentColor(QColor::fromRgb(rgb));
        emit sigMousePressSize(qAbs(e->y() - 255));
        mousePoint = e->pos();
        update();
    }
    isMousePress = true;
}

void RecSkinColorBrightnessRange::mouseReleaseEvent(QMouseEvent *)
{
    isMousePress = false;
}

void RecSkinColorBrightnessRange::mouseMoveEvent(QMouseEvent *e)
{
    if(isMousePress)
    {
        if(e->x() > 0 && e->x() < this->width() && e->y() < 255 && e->y() > 0)
        {
            QRgb rgb = drawImage.scaled(this->width(),this->height()).pixel(e->x(),e->y());
            emit currentColor(QColor::fromRgb(rgb));
            emit sigMousePressSize(qAbs(e->y() - 255));
            mousePoint = e->pos();
            update();
        }
    }
}


