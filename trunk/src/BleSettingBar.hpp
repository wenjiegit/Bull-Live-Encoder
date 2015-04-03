#ifndef BLESETTINGBAR_HPP
#define BLESETTINGBAR_HPP

#include <QWidget>

namespace Ui {
class BleSettingBar;
}

class BleSettingBar : public QWidget
{
    Q_OBJECT

public:
    explicit BleSettingBar(QWidget *parent = 0);
    ~BleSettingBar();

private:
    Ui::BleSettingBar *ui;
};

#endif // BLESETTINGBAR_HPP
