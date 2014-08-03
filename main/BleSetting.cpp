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

#include "BleSettingetting.hpp"
#include "ui_BleSetting.h"

#include <QCameraInfo>
#include <QSettings>

#include <stdint.h>
#include "x264.h"
#include "MOption.h"

static void addItem(QComboBox *box, const char * const * argv)
{
    for (int i = 0; ; ++i) {
        const char *name = argv[i];
        if (name != 0) {
            box->addItem(name);
        } else {
            break;
        }
    }
}

BleSetting::BleSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSetting)
{
    ui->setupUi(this);

    addItem(ui->x264Preset,  x264_preset_names);
    addItem(ui->x264Tune,    x264_tune_names);
    addItem(ui->x264Profile, x264_profile_names);

    // add res
    QSettings resIni("res.ini", QSettings::IniFormat);
    QStringList keys = resIni.allKeys();
    for (int i = 0; i < keys.size(); ++i) {
        const QString &key = keys.at(i);
        QStringList res_scale = key.split("_");
        if (res_scale.size() != 2) continue;

        ui->res->addItem(res_scale.join("  "), resIni.value(key));
    }

    // add bitrate mode list
    ui->bitrateMode->addItem("CBR");
    ui->bitrateMode->addItem("ABR");

    // add keyframe interval
    ui->keyFrameInterval->addItem("1");
    ui->keyFrameInterval->addItem("2");
    ui->keyFrameInterval->addItem("3");
    ui->keyFrameInterval->addItem("4");
    ui->keyFrameInterval->addItem("5");

    // add quality
    ui->quality->addItem("0");
    ui->quality->addItem("1");
    ui->quality->addItem("2");
    ui->quality->addItem("3");
    ui->quality->addItem("4");
    ui->quality->addItem("5");
    ui->quality->addItem("6");
    ui->quality->addItem("7");
    ui->quality->addItem("8");
    ui->quality->addItem("9");
    ui->quality->addItem("10");

    restore();

    connect(ui->applyBtn, SIGNAL(clicked()), this, SLOT(onApplyClicked()));
}

BleSetting::~BleSetting()
{
    delete ui;
}

void BleSetting::onApplyClicked()
{
    MOption *option = MOption::instance();

    QString format      = ui->format->currentText();
    QVariant res        = ui->res->itemData(ui->res->currentIndex());
    QString fps         = ui->fps->currentText();
    QString bitrate     = ui->bitrate->currentText();
    QString x264Preset  = ui->x264Preset->currentText();
    QString x264Tune    = ui->x264Tune->currentText();
    QString x264Profile = ui->x264Profile->currentText();
    QString address     = ui->address->text().trimmed();
    QString bitrateMode = ui->bitrateMode->currentText();
    QString keyFrameInterval    = ui->keyFrameInterval->currentText();
    QString quality             = ui->quality->currentText();

    // save
    option->setOption(format, "format", "encoder");
    option->setOption(res, "res", "encoder");
    option->setOption(fps, "fps", "encoder");
    option->setOption(bitrate, "bitrate", "encoder");

    option->setOption(x264Preset, "preset", "x264");
    option->setOption(x264Tune, "tune", "x264");
    option->setOption(x264Profile, "profile", "x264");
    option->setOption(bitrateMode, "BitrateMode", "x264");
    option->setOption(keyFrameInterval, "KeyFrameInterval", "x264");
    option->setOption(quality, "quality", "x264");

    option->setOption(address, "address", "network");
}

void BleSetting::restore()
{
    MOption *option = MOption::instance();

    // encoder group
    QString format      = option->option("format", "encoder").toString();
    QVariant res        = option->option("res", "encoder");
    QString fps         = option->option("fps", "encoder").toString();
    QString bitrate     = option->option("bitrate", "encoder").toString();
    ui->format->setCurrentIndex(ui->format->findText(format));
    ui->res->setCurrentIndex(ui->res->findData(res));
    ui->fps->setCurrentIndex(ui->fps->findText(fps));
    ui->bitrate->setCurrentIndex(ui->bitrate->findText(bitrate));

    // x264 group
    QString x264Preset  = option->option("preset", "x264").toString();
    QString x264Tune    = option->option("tune", "x264").toString();
    QString x264Profile = option->option("profile", "x264").toString();
    QString bitrateMode = option->option("BitrateMode", "x264").toString();
    QString keyFrameInterval = option->option("KeyFrameInterval", "x264").toString();
    QString quality = option->option("quality", "x264").toString();

    ui->x264Preset->setCurrentIndex(ui->x264Preset->findText(x264Preset));
    ui->x264Tune->setCurrentIndex(ui->x264Tune->findText(x264Tune));
    ui->x264Profile->setCurrentIndex(ui->x264Profile->findText(x264Profile));
    ui->bitrateMode->setCurrentIndex(ui->bitrateMode->findText(bitrateMode));
    ui->keyFrameInterval->setCurrentIndex(ui->keyFrameInterval->findText(keyFrameInterval));
    ui->quality->setCurrentIndex(ui->quality->findText(quality));

    // network group
    QString address     = option->option("address", "network").toString();

    ui->address->setText(address);
}

BleSettingDialog::BleSettingDialog(QWidget *parent)
    : MCustomDialog(parent)
{
    addWidget(new BleSetting(this));
}

BleSettingDialog::~BleSettingDialog()
{

}
