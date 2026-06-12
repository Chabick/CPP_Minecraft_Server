#include "PlayerManagement.h"

namespace System {
    std::vector<std::shared_ptr<MC::Player>> *PlayerManagement::players =
        new std::vector<std::shared_ptr<MC::Player>>();
    std::shared_ptr<const std::vector<std::shared_ptr<const MC::Player>>> PlayerManagement::playersPtr =
        std::make_shared<const std::vector<std::shared_ptr<const MC::Player>>>(
            std::vector<std::shared_ptr<const MC::Player>>(PlayerManagement::players->begin(), PlayerManagement::players->end()));

    std::shared_ptr<const std::vector<std::shared_ptr<const MC::Player>>> PlayerManagement::getPlayers() {
        //return std::make_unique<std::vector<std::shared_ptr<const MC::Player>>>(PlayerManagement::players.begin(), PlayerManagement::players.end());
        return PlayerManagement::playersPtr;
    }

    void PlayerManagement::registerPlayer(MC::Player* player) {
        PlayerManagement::players->push_back(std::make_shared<MC::Player>(*player));
    }

    void PlayerManagement::unregisterPlayer(MC::Player* player) {
        auto shptr = std::make_shared<MC::Player>(*player);
        for (auto it = PlayerManagement::players->begin(); it != PlayerManagement::players->end(); ++it) {
            if (*it == shptr) {
                PlayerManagement::players->erase(it);
                return;
            }
        }
    }

    bool PlayerManagement::isUsed() {
        return PlayerManagement::playersPtr.use_count() >= 2;
    }
} // System