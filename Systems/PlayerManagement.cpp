#include "PlayerManagement.h"

namespace System {
    std::unique_ptr<std::vector<std::shared_ptr<const MC::Player>>> PlayerManagement::getPlayers() {
        /*auto result = std::make_unique<std::vector<std::shared_ptr<const MC::Player>>>();

        result->reserve(players.size());

        for (const auto& player : players) {
            result->push_back(player);
        }

        return result;*/

        return std::make_unique<std::vector<std::shared_ptr<const MC::Player>>>(players.begin(), players.end());
    }

    void PlayerManagement::registerPlayer(MC::Player* player) {
        PlayerManagement::players.push_back(std::make_shared<MC::Player>(*player));
    }

    void PlayerManagement::unregisterPlayer(MC::Player* player) {
        auto shptr = std::make_shared<MC::Player>(*player);
        for (auto it = players.begin(); it != players.end(); ++it) {
            if (*it == shptr) {
                players.erase(it);
                return;
            }
        }
    }
} // System