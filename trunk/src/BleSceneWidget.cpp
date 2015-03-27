#include "BleSceneWidget.hpp"
#include "ui_BleSceneWidget.h"

#include "NcFramelessHelper.h"
#include "BleResizeableWidget.hpp"

BleSceneWidget::BleSceneWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSceneWidget)
{
    ui->setupUi(this);

    BleResizeableWidget *ssss = new BleResizeableWidget(this);
    ssss->resize(100, 100);
}

BleSceneWidget::~BleSceneWidget()
{
    delete ui;
}
