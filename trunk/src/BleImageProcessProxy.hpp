#ifndef BLEIMAGEPROCESSPROXY_HPP
#define BLEIMAGEPROCESSPROXY_HPP

#include <QWidget>

namespace Ui {
class BleImageProcessProxy;
}

class BleImageProcess;

class BleImageProcessProxy : public QWidget
{
    Q_OBJECT

public:
    explicit BleImageProcessProxy(QWidget *parent = 0);
    ~BleImageProcessProxy();

    void setImageProcess(BleImageProcess *processWidget);

    void resizeEvent(QResizeEvent * event);

private slots:
    void onSizeChanged();

private:
    void changeSize(QSize size);

private:
    Ui::BleImageProcessProxy *ui;
    BleImageProcess *m_processWidget;
};

#endif // BLEIMAGEPROCESSPROXY_HPP
