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
#include <QFileDialog>
#include <QPainter>

#include <stdint.h>
#include "x264.h"
#include "MOption.h"
#include "BleAudioCapture.hpp"
#include "BleLog.hpp"

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

static void insertItem(QStandardItem *item, QStandardItemModel &model)
{
    item->setSizeHint(QSize(130, 37));
    item->setEditable(false);
    model.appendRow(item);
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

    // add language
    ui->languageCombobox->addItem("English", "en.qm");
    ui->languageCombobox->addItem("简体中文", "zh.qm");
    ui->languageCombobox->addItem("繁体中文", "tw.qm");

    // audio format
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
    ui->bitrateMode->addItem("VBR");

    // add keyframe interval
    ui->keyFrameInterval->addItem("1");
    ui->keyFrameInterval->addItem("2");
    ui->keyFrameInterval->addItem("3");
    ui->keyFrameInterval->addItem("4");
    ui->keyFrameInterval->addItem("5");
    ui->keyFrameInterval->addItem("6");
    ui->keyFrameInterval->addItem("7");
    ui->keyFrameInterval->addItem("8");
    ui->keyFrameInterval->addItem("9");
    ui->keyFrameInterval->addItem("10");

    connect(ui->OKBtn, SIGNAL(clicked()), this, SLOT(onOKClicked()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
    connect(ui->applyBtn, SIGNAL(clicked()), this, SLOT(onApplyClicked()));
    connect(ui->qualityBar, SIGNAL(valueChanged(int)), this, SLOT(onQualityValueChanged(int)));
    connect(ui->browse, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
    connect(ui->enable_record, SIGNAL(stateChanged(int)), this, SLOT(onEnableSaveStateChanged(int)));

    restore();

    onQualityValueChanged(ui->qualityBar->value());
    onEnableSaveStateChanged(ui->enable_record->checkState());

    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));

    ui->listView->setFixedWidth(130);
    ui->listView->setModel(&m_model);
    ui->listView->setItemDelegate(new StyleItemDelegate(this));

    QString delim("   ");
    insertItem(new QStandardItem(delim + tr("Basic")), m_model);
    insertItem(new QStandardItem(delim + tr("Encoder")), m_model);
    insertItem(new QStandardItem(delim + tr("Network")), m_model);
    insertItem(new QStandardItem(delim + tr("Advanced")), m_model);
}

BleSetting::~BleSetting()
{
    delete ui;
}

void BleSetting::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QRect r(0, 0, width() - 4, height() - 4);
    r.moveCenter(rect().center());
    p.fillRect(r, Qt::white);
}

void BleSetting::onOKClicked()
{
    onApplyClicked();

    BleSettingDialog *d = dynamic_cast<BleSettingDialog *> (parentWidget());
    if (d) {
        d->accept();
    }
}

void BleSetting::onCancelClicked()
{
    BleSettingDialog *d = dynamic_cast<BleSettingDialog *> (parentWidget());
    if (d) {
        d->reject();
    }
}

void BleSetting::onApplyClicked()
{
    MOption *option = MOption::instance();

    // language
    QString qm = ui->languageCombobox->itemData(ui->languageCombobox->currentIndex()).toString();
    option->setOption(qm, "qm", "language");

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
    QString keyFrameInterval = ui->keyFrameInterval->currentText();
    QString threadCount = ui->threadCount->currentText();
    QString enableBFrame = ui->enableBFrame->isChecked() ? "true" : "false";
    QString B_frame_count = QString::number(ui->B_frame_count->value());
    QString quality             = QString::number(ui->qualityBar->value());

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
    option->setOption(threadCount, Key_Thread_Count, Group_X264);
    option->setOption(enableBFrame, Key_Enable_B_Frame, Group_X264);
    option->setOption(B_frame_count, Key_B_Frame_Count, Group_X264);
    option->setOption(quality, "quality", "x264");

    // video save
    option->setOption(ui->save_path->text(), "save_path", "Video_Save");
    QString enabled_record = ui->enable_record->isChecked() ? "true" : "false";
    option->setOption(enabled_record, "save_enabled", "Video_Save");

    option->setOption(address, "address", "network");

    emit settingChanged();
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

void BleSetting::onQualityValueChanged(int value)
{
    QString text = QString::number(value);
    ui->qualityLabel->setText(text);
}

void BleSetting::onBrowseClicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(0, tr("please select a directory"));
    if (dir_name.isEmpty()) return;

    ui->save_path->setText(dir_name);
}

void BleSetting::onEnableSaveStateChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    ui->save_path->setEnabled(enabled);
    ui->browse->setEnabled(enabled);
}

void BleSetting::onClicked(const QModelIndex &index)
{
    int row = index.row();
    if (row < 0) {
        return;
    }

    ui->stackedWidget->setCurrentIndex(row);
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
    QString threadCount = option->option(Key_Thread_Count, Group_X264).toString();
    QString enableBFrame = option->option(Key_Enable_B_Frame, Group_X264).toString();
    QString B_frame_count = option->option(Key_B_Frame_Count, Group_X264).toString();
    QString quality = option->option("quality", "x264").toString();

    setIndex(ui->x264Preset, x264Preset);
    setIndex(ui->x264Tune, x264Tune);
    setIndex(ui->x264Profile, x264Profile);
    setIndex(ui->bitrateMode, bitrateMode);
    setIndex(ui->keyFrameInterval, keyFrameInterval);
    setIndex(ui->threadCount, threadCount);
    ui->enableBFrame->setChecked((enableBFrame == "true") ? true: false);
    ui->B_frame_count->setValue(B_frame_count.toInt());
    ui->qualityBar->setValue(quality.toInt());

    // video save
    QString save_path = option->option("save_path", "Video_Save").toString();
    if (save_path.isEmpty()) {
        // set default to $app/record
        save_path = QCoreApplication::applicationDirPath() + "/record";
    }
    ui->save_path->setText(save_path);

    QString enabled_record_str = option->option("save_enabled", "Video_Save").toString();
    bool enabled_record = (enabled_record_str == "true") ? true : false;
    ui->enable_record->setChecked(enabled_record);

    // network group
    QString address = option->option("address", "network").toString();

    ui->address->setText(address);

    restoreLanguage();
}

void BleSetting::restoreLanguage()
{
    MOption *option = MOption::instance();
    QString lastqm = option->option("qm", "language").toString();
    if (lastqm.isEmpty()) {
        return;
    }

    int count = ui->languageCombobox->count();
    for (int i = 0; i < count; ++i) {
        QString qm = ui->languageCombobox->itemData(i).toString();
        if (lastqm == qm) {
            ui->languageCombobox->setCurrentIndex(i);
            QString language = ui->languageCombobox->itemText(i);
            log_trace("language set to %s", language.toStdString().c_str());
            break;
        }
    }
}

BleSettingDialog::BleSettingDialog(QWidget *parent)
    : MCustomDialog(parent)
{
    addWidget(new BleSetting(this));

    setTitle(tr("Setting Dialog"));
    setMovable(true);
    setResizable(false);

    setFixedSize(690, 500);
}

BleSettingDialog::~BleSettingDialog()
{

}
