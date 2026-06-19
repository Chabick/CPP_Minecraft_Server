#include "PlayerManagement.h"

namespace System {
    std::shared_ptr<std::vector<std::shared_ptr<MC::Player>>> PlayerManagement::players =
        std::make_shared<std::vector<std::shared_ptr<MC::Player>>>();
    std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> PlayerManagement::playersPtr = PlayerManagement::players;

    std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> PlayerManagement::getPlayers() {
        //return std::make_unique<std::vector<std::shared_ptr<const MC::Player>>>(PlayerManagement::players.begin(), PlayerManagement::players.end());
        return PlayerManagement::playersPtr;
    }

    void PlayerManagement::registerPlayer(std::shared_ptr<MC::Player> player) {
        PlayerManagement::players->push_back(player);
    }

    void PlayerManagement::unregisterPlayer(std::shared_ptr<MC::Player> player) {
        for (auto it = PlayerManagement::players->begin(); it != PlayerManagement::players->end(); ++it) {
            if (*it == player) {
                PlayerManagement::players->erase(it);
                return;
            }
        }
    }

    bool PlayerManagement::isUsed() {
        return PlayerManagement::playersPtr.use_count() >= 2;
    }
} // System