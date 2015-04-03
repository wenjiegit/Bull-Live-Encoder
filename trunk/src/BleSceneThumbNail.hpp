#ifndef BLESCENETHUMBNAIL_HPP
#define BLESCENETHUMBNAIL_HPP

#include <QWidget>
#include <QList>

namespace Ui {
class BleSceneThumbNail;
}

class BlePixmapItem;

class BleSceneThumbNail : public QWidget
{
    Q_OBJECT

public:
    explicit BleSceneThumbNail(QWidget *parent = 0);
    ~BleSceneThumbNail();

    void addThumbNail();
    void updateThumbNail(const QImage &image, int index);

private slots:
    void onClicked();

signals:
    void sceneChanged(int index);

private:
    Ui::BleSceneThumbNail *ui;
    QList<BlePixmapItem *> m_buttons;
};

#endif // BLESCENETHUMBNAIL_HPP
