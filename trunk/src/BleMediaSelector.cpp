#include "BleMediaSelector.hpp"
#include "ui_BleMediaSelector.h"

BleMediaSelector::BleMediaSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleMediaSelector)
{
    ui->setupUi(this);

    connect(ui->ok_btn, SIGNAL(clicked()), this, SIGNAL(ok()));
    connect(ui->cancel_btn, SIGNAL(clicked()), this, SIGNAL(cancel()));

    ui->addr->setFocus();
}

BleMediaSelector::~BleMediaSelector()
{
    delete ui;
}

QString BleMediaSelector::addr()
{
    return ui->addr->text();
}


BleMediaSelectorDialog::BleMediaSelectorDialog(QWidget *parent)
    : MCustomDialog(parent)
    , m_selector(new BleMediaSelector(this))
{
    addWidget(m_selector);
    setFixedSize(400, 120);
    setTitle(tr("add network addr."));

    connect(m_selector, SIGNAL(ok()), this, SLOT(onOK()));
    connect(m_selector, SIGNAL(cancel()), this, SLOT(onCancel()));
}

BleMediaSelectorDialog::~BleMediaSelectorDialog()
{

}

QString BleMediaSelectorDialog::addr()
{
    return m_selector->addr();
}

void BleMediaSelectorDialog::onOK()
{
    accept();
}

void BleMediaSelectorDialog::onCancel()
{
    reject();
}
