#ifndef MCSERVER_PLAYERMANAGEMENT_H
#define MCSERVER_PLAYERMANAGEMENT_H
#include <memory>
#include <vector>

#include "Minecraft/Player.h"

namespace System {
    class PlayerManagement {
        static std::vector<std::shared_ptr<MC::Player>> players;

    public:
        static void registerPlayer(MC::Player* player);
        static void unregisterPlayer(MC::Player* player);
        static std::unique_ptr<std::vector<std::shared_ptr<const MC::Player>>> getPlayers();
    };
} // System

#endif //MCSERVER_PLAYERMANAGEMENT_H