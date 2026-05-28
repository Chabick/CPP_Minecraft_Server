#ifndef MCSERVER_PLAYER_H
#define MCSERVER_PLAYER_H
#include "Location.h"

namespace MC {
    enum GameMode: int {
        SURVIVAL = 0, CREATIVE = 1, ADVENTURE = 2, SPECTATOR = 3, UNKNOWN = -1
    };

    class Player {
        int EID;
    public:
        MC::GameMode game_mode;
        MC::GameMode prev_game_mode;

        bool has_died = false;
        EntityLocation *death_location = nullptr;

        EntityLocation *location = nullptr;

        int portal_cooldown = 0;

        Player();
        ~Player();

        int getEID() {return EID;}
    };
}

#endif //MCSERVER_PLAYER_H