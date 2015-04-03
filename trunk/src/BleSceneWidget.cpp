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

#include "BleSceneWidget.hpp"
#include "ui_BleSceneWidget.h"

#include "NcFramelessHelper.h"
#include "BleResizeableWidget.hpp"
#include "BleImageProcess.hpp"
#include "BlePixmapItem.hpp"
#include "BleImageProcessThread.hpp"
#include "MOption.h"
#include "BleLog.hpp"
#include "BleImageProcessProxy.hpp"

static BleSceneWidget *gs_instance = NULL;

BleSceneWidget::BleSceneWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSceneWidget)
{
    ui->setupUi(this);

    gs_instance = this;

    addScene();
    addScene();
    addScene();
    addScene();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(ui->scenes, SIGNAL(sceneChanged(int)), this, SLOT(onSceneChanged(int)));

    m_timer.start(100);
}

BleSceneWidget::~BleSceneWidget()
{
    delete ui;
}

BleSceneWidget *BleSceneWidget::instance()
{
    return gs_instance;
}

void BleSceneWidget::addWidget(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
}

void BleSceneWidget::addScene()
{
    BleImageProcessThread *thread = new BleImageProcessThread(this);
    QSize si = MOption::instance()->option("res", "encoder").toSize();
    int fps = MOption::instance()->option("fps", "encoder").toInt();
    thread->setResolution(si.width(), si.height());
    thread->setInternal(1000 / fps);
    thread->start();

    BleImageProcessProxy *proxy = new BleImageProcessProxy(this);
    connect(this, SIGNAL(sizeChanged()), proxy, SLOT(onSizeChanged()));
    // image process
    BleImageProcess *processWidget = new BleImageProcess(proxy);
    processWidget->setProcessThread(thread);

    proxy->setImageProcess(processWidget);

    ui->stackedWidget->addWidget(proxy);
    ui->scenes->addThumbNail();

    // save scene
    ScenePair pair;
    pair.imageProcessWidget = processWidget;
    pair.imageProcessThread = thread;

    m_scenesPair[ui->stackedWidget->count() - 1] = pair;
}

BleImageProcess *BleSceneWidget::currentImageProcessWidget()
{
    int index = ui->stackedWidget->currentIndex();
    ScenePair pair = m_scenesPair.value(index);

    return pair.imageProcessWidget;
}

BleImageProcessThread *BleSceneWidget::currentImageProcessThread()
{
    int index = ui->stackedWidget->currentIndex();
    ScenePair pair = m_scenesPair.value(index);

    return pair.imageProcessThread;
}

void BleSceneWidget::onSceneChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    log_trace("scene changed to %d", index);
}

void BleSceneWidget::onTimeout()
{
    QHashIterator<int, ScenePair> i(m_scenesPair);
    while (i.hasNext()) {
        i.next();
        ScenePair pair = i.value();
        BleImage *image = pair.imageProcessThread->getImage();
        if (image && image->dataSize > 0) {
            QImage qimage(image->width, image->height, QImage::Format_RGB888);
            memcpy(qimage.bits(), image->data, qimage.byteCount());

            ui->scenes->updateThumbNail(qimage, i.key());
            delete image;
        }
    }
}
