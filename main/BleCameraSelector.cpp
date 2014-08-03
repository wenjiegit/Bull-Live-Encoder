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

#include "BleCameraSelector.hpp"
#include "ui_BleCameraSelector.h"

#include <QCameraInfo>

BleCameraSelector::BleCameraSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleCameraSelector)
{
    ui->setupUi(this);

    // camera list
    QList<QCameraInfo> infos = QCameraInfo::availableCameras();
    for (int i = 0; i < infos.size(); ++i) {
        QCameraInfo info = infos.at(i);
        ui->camera->addItem(info.description(), info.deviceName());
    }

    QDialog *dialog = dynamic_cast<QDialog*>(parent);
    if (dialog) {
        connect(ui->okBtn, SIGNAL(clicked()), dialog, SLOT(accept()));
        connect(ui->cancelBtn, SIGNAL(clicked()), dialog, SLOT(reject()));
    }
}

BleCameraSelector::~BleCameraSelector()
{
    delete ui;
}

QString BleCameraSelector::selectedCameraName()
{
    return ui->camera->itemData(ui->camera->currentIndex()).toString();
}

int BleCameraSelector::selectedCameraIndex()
{
    return ui->camera->currentIndex();
}

BleCameraSelectorDialog::BleCameraSelectorDialog(QWidget *parent)
    : MCustomDialog(parent)
    , m_selector(new BleCameraSelector(this))
{
    addWidget(m_selector);
    setFixedSize(320, 120);
    setTitle(tr("add camera"));
}

BleCameraSelectorDialog::~BleCameraSelectorDialog()
{

}

QString BleCameraSelectorDialog::selectedCameraName()
{
    return m_selector->selectedCameraName();
}

int BleCameraSelectorDialog::selectedCameraIndex()
{
    return m_selector->selectedCameraIndex();
}
