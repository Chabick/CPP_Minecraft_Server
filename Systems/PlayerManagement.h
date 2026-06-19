#ifndef MCSERVER_PLAYERMANAGEMENT_H
#define MCSERVER_PLAYERMANAGEMENT_H
#include <memory>
#include <vector>

#include "Minecraft/Player.h"

namespace System {
    class PlayerManagement {
        static std::shared_ptr<std::vector<std::shared_ptr<MC::Player>>> players;
        static std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> playersPtr;

    public:
        static bool isUsed();
        static void registerPlayer(std::shared_ptr<MC::Player> player);
        static void unregisterPlayer(std::shared_ptr<MC::Player> player);
        static std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> getPlayers();
    };
} // System

#endif //MCSERVER_PLAYERMANAGEMENT_H