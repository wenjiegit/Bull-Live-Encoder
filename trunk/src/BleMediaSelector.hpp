#ifndef BLEMEDIASELECTOR_HPP
#define BLEMEDIASELECTOR_HPP

#include <QWidget>

#include "MCustomDialog.h"

namespace Ui {
class BleMediaSelector;
}

class BleMediaSelector : public QWidget
{
    Q_OBJECT

public:
    explicit BleMediaSelector(QWidget *parent = 0);
    ~BleMediaSelector();

    QString addr();

signals:
    void ok();
    void cancel();

private:
    Ui::BleMediaSelector *ui;
};

class BleMediaSelectorDialog : public MCustomDialog
{
    Q_OBJECT

public:
    explicit BleMediaSelectorDialog(QWidget *parent = 0);
    ~BleMediaSelectorDialog();

    QString addr();

private slots:
    void onOK();
    void onCancel();

private:
    BleMediaSelector * m_selector;
};

#endif // BLEMEDIASELECTOR_HPP
