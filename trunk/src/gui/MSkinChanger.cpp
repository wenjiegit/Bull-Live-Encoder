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

#include "MSkinChanger.h"
#include "ui_MSkinChanger.h"

#include <QFileDialog>
#include <QPainter>
#include <QDebug>

#include "MOption.h"
#include "NcFramelessHelper.h"
#include "RecSkinPickerWidget.h"
#include "RecSkinWidget.h"
#include "RecSkinDialog.h"

MSkinChanger::MSkinChanger(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MSkinChanger)
    , SkinDialog(0)
{
    ui->setupUi(this);
    SkinWidget = new RecSkinWidget(this);

    SkinPickerWidget = new RecSkinPickerWidget(this);

    connect(SkinWidget,SIGNAL(currentPixmap(QString,QPixmap,QColor)),
            this,SLOT(onCurrentPixmap(QString,QPixmap,QColor)));

    connect(this,SIGNAL(currentColor(QColor)),
            SkinPickerWidget,SIGNAL(PixColor(QColor)));

    connect(SkinWidget,SIGNAL(definePixmapResult()),
            this,SLOT(onDefinePixmap()));

    ui->tabBar->addItem(tr("picture"));
    ui->tabBar->addItem(tr("color"));
    ui->stackedWidget->addWidget(SkinWidget);
    ui->stackedWidget->addWidget(SkinPickerWidget);
    connect(ui->tabBar,SIGNAL(currentChanged(int)),
            ui->stackedWidget,SLOT(setCurrentIndex(int)));

    connect(ui->areoTransparent, SIGNAL(valueChanged(int)), this, SLOT(onAeroTransparentValueChanged(int)));
    connect(ui->widgetTransparent, SIGNAL(valueChanged(int)), this, SLOT(onWidgetTransparentValueChanged(int)));

    int aeroTransparent = MOption::instance()->option("AeroTransparent", "theme").toInt();
    int widgetTransparent = MOption::instance()->option("WidgetTransparent", "theme").toInt();
    ui->areoTransparent->setValue(aeroTransparent);
    ui->widgetTransparent->setValue(widgetTransparent);
}

MSkinChanger::~MSkinChanger()
{
    delete ui;
}

void MSkinChanger::onCurrentPixmap(const QString &fileName,const QPixmap &pix,const QColor &averageColor)
{
    QString basePath = QString(fileName).remove(QCoreApplication::applicationDirPath());
    MOption::instance()->setOption(basePath, "WindowBGPixmap", "theme");
    MOption::instance()->setOption("bitmap", "WindowBGPixmapType", "theme");
    MOption::instance()->setOption(QVariant(averageColor), OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme);

    updateThemedWidgets();
    emit currentPixmap(fileName,pix,averageColor);
    emit currentColor(averageColor);
    updateTheme();
}

void MSkinChanger::onAeroTransparentValueChanged(int v)
{
    float value = qAbs(v - 50 - 205);
    QString text(QString::number(value/205 * 100,'f',2) + "%");
    ui->areoLabel->setText(text);
    MOption::instance()->setOption(v, "AeroTransparent", "theme");
    updateThemedWidgets();
    updateTheme();
}

void MSkinChanger::onWidgetTransparentValueChanged(int v)
{
    float value = qAbs(v - 50 - 205);
    QString text(QString::number(value/205 * 100,'f',2) + "%");
    ui->widgetLabel->setText(text);
    MOption::instance()->setOption(v, "WidgetTransparent", "theme");
    updateThemedWidgets();
}

void MSkinChanger::onDefinePixmap()
{
    QWidget *desktopWidget = 0;
    foreach (QWidget* w, qApp->allWidgets()) {
        if(w->objectName() == "BleMainWindow")
            desktopWidget = w;
    }
    Q_ASSERT(desktopWidget);
    QPixmap mainPix = QPixmap::grabWidget(desktopWidget);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Images (*.png *.bmp *.jpg)"));
    if(fileName.isEmpty())
        return;

    QImage skinImage;
    skinImage.load(fileName);
    skinImage = skinImage.scaled(1366, 768);

    if (!SkinDialog) {
        SkinDialog = new RecSkinDialog(mainPix.toImage(),skinImage);
        connect(SkinDialog, SIGNAL(sigSavePix(QString, QPixmap, QColor)),
                SkinWidget, SLOT(onSavePix(QString, QPixmap, QColor)));
    } else {
        SkinDialog->setImage(mainPix.toImage(),skinImage);
    }

    SkinDialog->showFullScreen();
}

MSkinChangerWidget::MSkinChangerWidget(QWidget *parent)
    : QjtCustomWidget(parent)
{
    addWidget(new MSkinChanger(this));
    setTitle(tr("change skin"));
    setFixedSize(490, 380);
    setResizable(false);

    setPaintHeight(76, 0, 0);
}
