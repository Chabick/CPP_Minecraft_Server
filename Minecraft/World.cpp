#include "World.h"
#include "Player.h"

namespace MC {
    SDynamicConfig DynamicConfig = SDynamicConfig();
}

MC::Difficulty MC::SWorldConfig::difficulty = MC::Difficulty::EASY;
int MC::SWorldConfig::view_distance = 16;
int MC::SWorldConfig::simulation_distance = 16;
bool MC::SWorldConfig::is_hardcore = false;
bool MC::SWorldConfig::enable_respawn_screen = true;
std::vector<std::string> MC::SWorldConfig::dimension_ids = {"minecraft:overworld",};
bool MC::SWorldConfig::do_limited_crafting = false;
//id (from the registers) from the dimension being spawned in
int MC::SWorldConfig::spawndim_num_id = findRegId(registry_lengths[1],
    &registry_dimensionTypes, "minecraft:overworld");
std::string MC::SWorldConfig::spawndim_id = "minecraft:overworld";
int64_t MC::SWorldConfig::hashed_seed = 0;
MC::GameMode MC::SWorldConfig::standard_game_mode = MC::GameMode::SURVIVAL;
bool MC::SWorldConfig::reduced_debug_info = false;
bool MC::SWorldConfig::is_debug = false;
bool MC::SWorldConfig::is_flat = false;
MC::World* MC::SWorldConfig::world = new MC::World(); //World will load all data later