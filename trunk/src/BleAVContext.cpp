#include "BleAVContext.hpp"

BleAVContext::BleAVContext()
{
    captureThread = 0;
}

BleAVContext *BleAVContext::instance()
{
    static BleAVContext *ret = 0;
    if (!ret) {
        ret = new BleAVContext;
    }

    return ret;
}

void BleAVContext::addPlayer(BleVLCPlayer *player)
{
    m_players << player;
}

void BleAVContext::removePlayer(BleVLCPlayer *player)
{
    m_players.removeAll(player);
}

QList<BleVLCPlayer *> BleAVContext::getPlayers()
{
    return m_players;
}
