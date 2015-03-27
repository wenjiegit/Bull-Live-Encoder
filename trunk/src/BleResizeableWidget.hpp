#ifndef BLERESIZEABLEWIDGET_HPP
#define BLERESIZEABLEWIDGET_HPP

#include <QWidget>

class BleResizeableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BleResizeableWidget(QWidget *parent = 0);

    void setEditable(bool enable);

    void paintEvent(QPaintEvent *e);

signals:

public slots:

private:
    bool m_editable;
};

#endif // BLERESIZEABLEWIDGET_HPP
