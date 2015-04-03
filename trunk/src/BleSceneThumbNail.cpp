#include "BleSceneThumbNail.hpp"
#include "ui_BleSceneThumbNail.h"

#include "BlePixmapItem.hpp"

#include <QDebug>

BleSceneThumbNail::BleSceneThumbNail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSceneThumbNail)
{
    ui->setupUi(this);
}

BleSceneThumbNail::~BleSceneThumbNail()
{
    delete ui;
}

void BleSceneThumbNail::addThumbNail()
{
    ui->verticalLayout->removeItem(ui->verticalSpacer);
    BlePixmapItem *item = new BlePixmapItem(this);
    ui->verticalLayout->addWidget(item);
    ui->verticalLayout->addItem(ui->verticalSpacer);

    connect(item, SIGNAL(clicked()), this, SLOT(onClicked()));
    // save
    m_buttons << item;
}

void BleSceneThumbNail::updateThumbNail(const QImage &image, int index)
{
    m_buttons.at(index)->setPixmap(QPixmap::fromImage(image));
}

void BleSceneThumbNail::onClicked()
{
    BlePixmapItem *item = dynamic_cast<BlePixmapItem *> (sender());
    if (!item) {
        return;
    }

    int index = m_buttons.indexOf(item);
    if (index >= 0) {
        emit sceneChanged(index);
    }
}
