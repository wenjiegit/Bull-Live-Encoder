#ifndef BLENETSTREAMSOURCE_HPP
#define BLENETSTREAMSOURCE_HPP

#include "BleSourceAbstract.hpp"
#include "BleThread.hpp"

#include <QMutex>

class BleVLCPlayer;

class BleNetStreamSource : public BleThread, public BleSourceAbstract
{
    Q_OBJECT
public:
    BleNetStreamSource(QObject * parent = 0);

    virtual QString getSourceName();
    virtual QImage getImage();
    virtual void stopCapture();
    virtual void run();

    void setMedia(const QString &fileName);

private:
    BleVLCPlayer *m_vlcPlayer;
};

#endif // BLENETSTREAMSOURCE_HPP
