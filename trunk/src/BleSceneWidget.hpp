#ifndef BLESCENEWIDGET_HPP
#define BLESCENEWIDGET_HPP

#include <QWidget>

namespace Ui {
class BleSceneWidget;
}

class BleSceneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BleSceneWidget(QWidget *parent = 0);
    ~BleSceneWidget();

private:
    Ui::BleSceneWidget *ui;
};

#endif // BLESCENEWIDGET_HPP
