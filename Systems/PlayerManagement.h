#ifndef MCSERVER_PLAYERMANAGEMENT_H
#define MCSERVER_PLAYERMANAGEMENT_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "Minecraft/Player.h"
#include "IDManagement.h"

namespace System {
    class PlayerManagement {

    public:
        class PlayerT { //formally AccessGuard
            friend class PlayerManagement;

        public:
            PlayerT(const PlayerT&) = delete;
            PlayerT& operator=(const PlayerT&) = delete;

            PlayerT(PlayerT&& other) noexcept;

            ~PlayerT();

            PlayerT& operator&(const PlayerT&) noexcept;
            PlayerT& operator&(PlayerT&& other) = delete;

            MC::Player* operator->() const { return player.get(); }
            MC::Player& operator*() = delete;

        private:
            std::shared_ptr<MC::Player> player;
            PlayerT(std::shared_ptr<MC::Player> player);
        };

    private:
        static std::unordered_map<Id, std::shared_ptr<MC::Player>> players;
        static std::vector<Id> playerIds;
        static int usage;


        //static std::shared_ptr<std::vector<std::shared_ptr<MC::Player>>> players;
        //static std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> playersPtr;

    public:
        static bool isUsed();
        static Id registerPlayer(MC::Player* player);
        static void unregisterPlayer(Id id);
        static std::vector<Id> const * getPlayers();
        static PlayerT getPlayer(Id id);
        static std::unordered_map<Id, std::shared_ptr<MC::Player>> const * __getPlayers__();
    };
} // System

#endif //MCSERVER_PLAYERMANAGEMENT_H