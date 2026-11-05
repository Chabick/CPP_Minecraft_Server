#include "PlayerManagement.h"
#include "Systems/IDManagement.h"

#include <algorithm>

#include "Util/Logger.h"

namespace System {

    int PlayerManagement::usage = 0;

    std::unordered_map<Id, std::shared_ptr<MC::Player>> PlayerManagement::players{};

    std::vector<Id> PlayerManagement::playerIds{};

    std::unordered_map<Id, std::shared_ptr<MC::Player>> const * PlayerManagement::__getPlayers__() {
        return &players;
    }

    Id PlayerManagement::registerPlayer(MC::Player* player) {
        player->player_id = System::IDManagement::gen();
        players.emplace(player->player_id, player);
        playerIds.insert(playerIds.end(), player->player_id);
        return player->player_id;
    }

    void PlayerManagement::unregisterPlayer(Id playerId) {
        players.erase(playerId);
        playerIds.erase(std::ranges::find(playerIds, playerId));
    }

    std::vector<Id> const * PlayerManagement::getPlayers() {
        return &PlayerManagement::playerIds;
    }

    PlayerManagement::PlayerT PlayerManagement::getPlayer(const Id id) {
        PlayerManagement::PlayerT player{players.at(id)};
        return player;
    }

    bool PlayerManagement::isUsed() {
        return usage > 0;
    }

    PlayerManagement::PlayerT::PlayerT(std::shared_ptr<MC::Player> player) {
        this->player = player;
        PlayerManagement::usage++;
    }

    PlayerManagement::PlayerT::~PlayerT() {
        PlayerManagement::usage--;
    }

    /*
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
    }*/
} // System