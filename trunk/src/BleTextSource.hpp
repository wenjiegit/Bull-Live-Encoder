#ifndef BLETEXTSOURCE_HPP
#define BLETEXTSOURCE_HPP

#include <QObject>
#include <QTimer>
#include <QImage>

#include "BleSourceAbstract.hpp"

class BleTextSource : public QObject, public BleSourceAbstract
{
    Q_OBJECT
public:
    explicit BleTextSource(QObject *parent = 0);

    virtual QString getSourceName();
    virtual QImage getImage();
    virtual void stopCapture();
    void setText(const QString &text);

private slots:
    void onTimeout();

private:
    BleImage m_image;
    QTimer m_timer;
    QString m_text;
    QImage m_textImage;
};

#endif // BLETEXTSOURCE_HPP
