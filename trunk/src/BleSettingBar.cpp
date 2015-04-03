#include "BleSettingBar.hpp"
#include "ui_BleSettingBar.h"

BleSettingBar::BleSettingBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleSettingBar)
{
    ui->setupUi(this);
}

BleSettingBar::~BleSettingBar()
{
    delete ui;
}
