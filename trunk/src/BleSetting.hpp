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

#ifndef BLESETTINGETTING_H
#define BLESETTINGETTING_H

#include <QWidget>

#include "MCustomDialog.h"

namespace Ui {
class BleSetting;
}

class BleSetting : public QWidget
{
    Q_OBJECT

public:
    explicit BleSetting(QWidget *parent = 0);
    ~BleSetting();

private slots:
    void onApplyClicked();
    void onAudioBitrateChanged(const QString & text);
    void onQualityValueChanged(int value);
    void onBrowseClicked();
    void onEnableSaveStateChanged(int state);

private:
    void restore();

signals:
    void start();
    void stop();
    void settingChanged();

private:
    Ui::BleSetting *ui;
};

class BleSettingDialog : public MCustomDialog
{
    Q_OBJECT
public:
    explicit BleSettingDialog(QWidget *parent = 0);
    ~BleSettingDialog();
};

#endif // BLESETTINGETTING_H
