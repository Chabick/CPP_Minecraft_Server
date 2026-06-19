#ifndef MCSERVER_PLAYER_H
#define MCSERVER_PLAYER_H
#include <iostream>
#include <ostream>

#include "Location.h"
#include <string>

struct Client;

namespace MC {
    enum GameMode: int {
        SURVIVAL = 0, CREATIVE = 1, ADVENTURE = 2, SPECTATOR = 3, UNKNOWN = -1
    };

    class Player {
        int EID;
        uint8_t uuid[16];
        Client *parent;
    public:
        MC::GameMode game_mode;
        MC::GameMode prev_game_mode;

        bool has_died = false;
        EntityLocation *death_location = nullptr;

        EntityLocation *location = nullptr;

        std::string name;

        int portal_cooldown = 0;

        Player(uint8_t uuid[], Client *parent);
        ~Player();

        int getEID() {return EID;}
        [[nodiscard]] const uint8_t* getUUId() {return uuid;};

        bool operator==(const Player &p) const {return name==p.name && EID==p.EID;};
    };
}

#endif //MCSERVER_PLAYER_H