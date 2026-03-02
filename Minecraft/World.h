#ifndef MCSERVER_WORLD_H
#define MCSERVER_WORLD_H
#include <cstdint>
#include <string>
#include <vector>

#include "Minecraft.h"

namespace MC {
    enum Difficulty {
        PEACEFUL = 0, EASY = 1, NORMAL = 2, HARD = 3
    };

    enum GameMode {
        SURVIVAL = 0, CREATIVE = 1, ADVENTURE = 2, SPECTATOR = 3, UNKNOWN = -1
    };

    class World {
        std::vector<MC::Player*> players; //players that are currently connected
        std::vector<MC::Player*> offline_players; //players that were online but disconnected
    };

    struct SWorldConfig {
        Difficulty difficulty = Difficulty::EASY;
        int view_distance = 16;
        int simulation_distance = 16;
        bool is_hardcore = false; //Not implemented
        bool enable_respawn_screen = true; //otherwise not implemented
        std::string dimension_ids[1] = {"minecraft:overworld"};
        bool do_limited_crafting = false; //unused by the client
        int spawndim_num_id = 0; //id (from the registers) from the dimension being spawned in
        std::string spawndim_id = "minecraft:overworld";
        int64_t hashed_seed = 0; //Not implemented
        GameMode standard_game_mode = GameMode::SURVIVAL;
        bool is_debug = false; //otherwise not implemented
        bool is_flat = false; //just clientside cosmetic
        World* world;
    } WorldConfig;
}

#endif //MCSERVER_WORLD_H