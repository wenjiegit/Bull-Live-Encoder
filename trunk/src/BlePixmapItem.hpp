#ifndef STCPIXMAPITEM_HPP
#define STCPIXMAPITEM_HPP

#include <QPushButton>

namespace Ui {
class BlePixmapItem;
}

class BlePixmapItem : public QPushButton
{
    Q_OBJECT
public:
    explicit BlePixmapItem(QWidget *parent = 0);
    ~BlePixmapItem();

    void setPixmap(const QPixmap &pix);
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent * event);

    virtual QSize sizeHint() const;

    void setSelected(bool selected);

private slots:
    void onPressed();

private:
    Ui::BlePixmapItem *ui;
    bool m_enter;
    bool m_selected;

    QPixmap m_icon;
};

#endif // STCPIXMAPITEM_HPP
