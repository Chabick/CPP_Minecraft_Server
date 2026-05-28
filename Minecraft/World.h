#ifndef MCSERVER_WORLD_H
#define MCSERVER_WORLD_H
#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "Chunk.h"
#include "Location.h"
#include "Data/Registries.h"
//#include "Player.h"


namespace MC {
    class Player; //forward declaration
    enum GameMode : int;

    enum Difficulty {
        PEACEFUL = 0, EASY = 1, NORMAL = 2, HARD = 3
    };

    class World {
    public:
        std::vector<MC::Player*> players; //players that are currently connected
        std::vector<MC::Player*> offline_players; //players that were online but disconnected
        std::vector<MC::ChunkColumn> chunk_columns;
        std::map<MC::ChunkColumnLocation, MC::ChunkColumn*> p_chunk_columns;
        int sea_level = 0; //not implemented
    };

    struct SWorldConfig {
        static Difficulty difficulty;
        static int view_distance;
        static int simulation_distance;
        static bool is_hardcore; //Not implemented, leave on false
        static bool enable_respawn_screen; //otherwise not implemented, leave on true
        static std::vector<std::string> dimension_ids;
        static bool do_limited_crafting; //unused by the client
        static int spawndim_num_id;//0;
        static std::string spawndim_id;
        static int64_t hashed_seed; //Not implemented
        static GameMode standard_game_mode;
        static bool reduced_debug_info;
        static bool is_debug; //otherwise not implemented
        static bool is_flat; //just clientside cosmetic
        static World* world;
    };

    struct SDynamicConfig {
        int playercount = 0;
    };

    extern SDynamicConfig DynamicConfig;
}

#endif //MCSERVER_WORLD_H