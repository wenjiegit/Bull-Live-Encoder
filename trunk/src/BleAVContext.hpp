#ifndef BLEAVCONTEXT_HPP
#define BLEAVCONTEXT_HPP

#include "BleImageCaptureThread.hpp"

#include <QList>

class BleVLCPlayer;

class BleAVContext
{
public:
    BleAVContext();

    static BleAVContext *instance();

    BleImageCaptureThread *captureThread;

    // for player audio samples
    void addPlayer(BleVLCPlayer *player);
    void removePlayer(BleVLCPlayer *player);
    QList<BleVLCPlayer*> getPlayers();

private:
    QList<BleVLCPlayer*> m_players;
};

#endif // BLEAVCONTEXT_HPP
