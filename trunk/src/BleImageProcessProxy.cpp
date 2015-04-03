#include "BleImageProcessProxy.hpp"
#include "ui_BleImageProcessProxy.h"

#include <QResizeEvent>

#include "MOption.h"
#include "BleImageProcess.hpp"

BleImageProcessProxy::BleImageProcessProxy(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BleImageProcessProxy)
    , m_processWidget(NULL)
{
    ui->setupUi(this);
}

BleImageProcessProxy::~BleImageProcessProxy()
{
    delete ui;
}

void BleImageProcessProxy::setImageProcess(BleImageProcess *processWidget)
{
    m_processWidget = processWidget;
}

void BleImageProcessProxy::resizeEvent(QResizeEvent *event)
{
    changeSize(event->size());
}

void BleImageProcessProxy::onSizeChanged()
{
    changeSize(size());
}

void BleImageProcessProxy::changeSize(QSize size)
{
    int w = size.width();
    int h = size.height();

    QSize outputSize = MOption::instance()->option("res", "encoder").toSize();

    int pw;
    int ph;

    if (outputSize.width() * h > outputSize.height() * w) {
        pw = w;
        ph = outputSize.height() * w / outputSize.width();
    } else {
        ph = h;
        pw = outputSize.width() * h / outputSize.height();
    }

    if (m_processWidget) {
        QRect r(0, 0, pw, ph);
        r.moveCenter(rect().center());
        m_processWidget->setGeometry(r);
    }
}
