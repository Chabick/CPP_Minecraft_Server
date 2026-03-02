#ifndef MCSERVER_PLAYER_H
#define MCSERVER_PLAYER_H

namespace MC {
    class Player {
        MC::GameMode game_mode = MC::GameMode::SURVIVAL;
        MC::GameMode prev_game_mode = MC::GameMode::UNKNOWN;
    };
}

#endif //MCSERVER_PLAYER_H