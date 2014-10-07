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

#include "BleSetting.hpp"
#include "ui_BleSetting.h"

#include <QCameraInfo>
#include <QSettings>

#include <stdint.h>
#include "x264.h"
#include "MOption.h"
#include "BleAudioCapture.hpp"

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

static void setIndex(QComboBox *box, const QString &str)
{
    int index = box->findText(str);
    if (index >= 0) {
        box->setCurrentIndex(index);
    }
}

BleSetting::BleSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSetting)
{
    ui->setupUi(this);

    connect(ui->audioSampleRate, SIGNAL(currentIndexChanged(QString))
            , this, SLOT(onAudioBitrateChanged(QString)));
    connect(ui->audioChannels, SIGNAL(currentIndexChanged(QString))
            , this, SLOT(onAudioBitrateChanged(QString)));

    // add audio info
    QHash<int, QString> audioDevices = BleAudioCapture::availableDevices();
    QHash<int, QString>::iterator iter = audioDevices.begin();
    while (iter != audioDevices.end()) {
        ui->audioDevice->addItem(iter.value(), QVariant(iter.key()));
        ++iter;
    }

    ui->audioFormat->addItem("AAC");
    ui->audioFormat->addItem("MP3");

    ui->audioChannels->addItem("Stereo");
    ui->audioChannels->addItem("Mono");

    ui->audioSampleRate->addItem("44100");
    ui->audioSampleRate->addItem("22050");
    ui->audioSampleRate->addItem("11025");

    addItem(ui->x264Preset,  x264_preset_names);
    addItem(ui->x264Tune,    x264_tune_names);
    addItem(ui->x264Profile, x264_profile_names);

    // add default tune, which is NULL
    ui->x264Tune->insertItem(0, "Default");

    // add default profile, which is not set
    // @see http://mewiki.project357.com/wiki/X264_Settings
    ui->x264Profile->insertItem(0, "Default");

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
    ui->keyFrameInterval->addItem("5");
    ui->keyFrameInterval->addItem("6");
    ui->keyFrameInterval->addItem("7");
    ui->keyFrameInterval->addItem("8");
    ui->keyFrameInterval->addItem("9");
    ui->keyFrameInterval->addItem("10");

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

    // audio group
    int audioDevID = ui->audioDevice->itemData(ui->audioDevice->currentIndex()).toInt();
    QString audioFormat = ui->audioFormat->currentText();
    QString audioChannels = ui->audioChannels->currentText();
    QString audioSampleRate = ui->audioSampleRate->currentText();
    QString audioBitrate = ui->audioBitrate->currentText();

    option->setOption(audioDevID, "dev_id", "audio");
    option->setOption(audioFormat, "format", "audio");
    option->setOption(audioChannels, "channels", "audio");
    option->setOption(audioSampleRate, "sample_rate", "audio");
    option->setOption(audioBitrate, "bitrate", "audio");

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

void BleSetting::onAudioBitrateChanged(const QString &text)
{
    Q_UNUSED(text);

    QString channels = ui->audioChannels->currentText();
    QString sampleRate = ui->audioSampleRate->currentText();

    if (channels == "Mono" && sampleRate == "44100") {              // one channel, 44.1 KHZ
        ui->audioBitrate->clear();

        ui->audioBitrate->addItem("224");
        ui->audioBitrate->addItem("192");
        ui->audioBitrate->addItem("160");
        ui->audioBitrate->addItem("128");
        ui->audioBitrate->addItem("112");
        ui->audioBitrate->addItem("96");
        ui->audioBitrate->addItem("80");
        ui->audioBitrate->addItem("64");
        ui->audioBitrate->addItem("56");

        ui->audioBitrate->setCurrentText("56");
    } else if (channels == "Mono" && sampleRate == "22050") {       // one channel, 22.05 KHZ
        ui->audioBitrate->clear();

        ui->audioBitrate->addItem("48");
        ui->audioBitrate->addItem("40");
        ui->audioBitrate->addItem("32");

        ui->audioBitrate->setCurrentText("48");
    } else if (channels == "Mono" && sampleRate == "11025") {       // one channel, 11.025 KHZ
        ui->audioBitrate->addItem("20");
        ui->audioBitrate->addItem("18");

        ui->audioBitrate->setCurrentText("20");
    } else if (channels == "Stereo" && sampleRate == "44100") {     // two channel, 44.1 KHZ
        ui->audioBitrate->clear();

        ui->audioBitrate->addItem("224");
        ui->audioBitrate->addItem("192");
        ui->audioBitrate->addItem("160");
        ui->audioBitrate->addItem("128");
        ui->audioBitrate->addItem("112");
        ui->audioBitrate->addItem("96");

        ui->audioBitrate->setCurrentText("96");
    } else if (channels == "Stereo" && sampleRate == "22050") {     // two channel, 22.05 KHZ
        ui->audioBitrate->clear();

        ui->audioBitrate->addItem("80");
        ui->audioBitrate->addItem("64");
        ui->audioBitrate->addItem("56");
        ui->audioBitrate->addItem("48");
        ui->audioBitrate->addItem("40");

        ui->audioBitrate->setCurrentText("80");
    } else if (channels == "Stereo" && sampleRate == "11025") {     // two channel, 11.025 KHZ
        ui->audioBitrate->clear();

        ui->audioBitrate->addItem("32");
        ui->audioBitrate->addItem("24");
        ui->audioBitrate->addItem("20");

        ui->audioBitrate->setCurrentText("32");
    }
}

void BleSetting::restore()
{
    MOption *option = MOption::instance();

    // audio group
    int audioDevID = option->option("dev_id", "audio").toInt();
    QString audioFormat = option->option("format", "audio").toString();
    QString audioChannels = option->option("channels", "audio").toString();
    QString audioSampleRate = option->option("sample_rate", "audio").toString();
    QString audioBitrate = option->option("sample_rate", "audio").toString();

    // find dev ID
    int audioDevCount = ui->audioDevice->count();
    for (int i = 0; i < audioDevCount; ++i) {
        if (ui->audioDevice->itemData(i).toInt() == audioDevID) {
            ui->audioDevice->setCurrentIndex(i);
            break;
        }
    }

    setIndex(ui->audioFormat, audioFormat);
    setIndex(ui->audioChannels, audioChannels);
    setIndex(ui->audioSampleRate, audioSampleRate);
    setIndex(ui->audioBitrate, audioBitrate);

    // encoder group
    QString format      = option->option("format", "encoder").toString();
    QVariant res        = option->option("res", "encoder");
    QString fps         = option->option("fps", "encoder").toString();
    QString bitrate     = option->option("bitrate", "encoder").toString();

    setIndex(ui->format, format);

    ui->res->setCurrentIndex(ui->res->findData(res));

    setIndex(ui->fps, fps);
    setIndex(ui->bitrate, bitrate);

    // x264 group
    QString x264Preset  = option->option("preset", "x264").toString();
    QString x264Tune    = option->option("tune", "x264").toString();
    QString x264Profile = option->option("profile", "x264").toString();
    QString bitrateMode = option->option("BitrateMode", "x264").toString();
    QString keyFrameInterval = option->option("KeyFrameInterval", "x264").toString();
    QString quality = option->option("quality", "x264").toString();

    setIndex(ui->x264Preset, x264Preset);
    setIndex(ui->x264Tune, x264Tune);
    setIndex(ui->x264Profile, x264Profile);
    setIndex(ui->bitrateMode, bitrateMode);
    setIndex(ui->keyFrameInterval, keyFrameInterval);
    setIndex(ui->quality, quality);

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
