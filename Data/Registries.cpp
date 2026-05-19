#include "Registries.h"

#include <array>
#include <vector>

int registry_lengths[] = {
    25,
    1, //dimension types
    1,
    1, //cat variant
    1, //chicken
    1, //cow
    1, //frog
    4, //painting
    1, //pig
    1, //wolf sounds
    1, //wolf variant
    1, //zombie nautilus
    //3, //timeline
};

//#define REG_COUNT 12
const char *registries[] = {
    "minecraft:damage_type",
    "minecraft:dimension_type",
    "minecraft:worldgen/biome",
    "minecraft:cat_variant",
    "minecraft:chicken_variant",
    "minecraft:cow_variant",
    "minecraft:frog_variant",
    "minecraft:painting_variant",
    "minecraft:pig_variant",
    "minecraft:wolf_sound_variant",
    "minecraft:wolf_variant",
    "minecraft:zombie_nautilus_variant",
    //"minecraft:timeline"
};

void* registry_pointers[] = {
    &registry_damageTypes,
    &registry_dimensionTypes,
    &registry_biomeTypes,
    &registry_cat_variant,
    &registry_chicken_variant,
    &registry_cow_variant,
    &registry_frog_variant,
    &registry_painting_variant,
    &registry_pig_variant,
    &registry_wolf_sound_variant,
    &registry_wolf_variant,
    &registry_zombie_nautilus_variant,
    //&registry_timeline
};

//#define REG_LENGTH_DAMAGE_TYPES 25
const char *registry_damageTypes[] = {
    "minecraft:cactus",
    "minecraft:campfire",
    "minecraft:cramming",
    "minecraft:dragon_breath",
    "minecraft:drown",
    "minecraft:dry_out",
    "minecraft:ender_pearl",
    "minecraft:fall",
    "minecraft:fly_into_wall",
    "minecraft:freeze",
    "minecraft:generic",
    "minecraft:generic_kill",
    "minecraft:hot_floor",
    "minecraft:in_fire",
    "minecraft:in_wall",
    "minecraft:lava",
    "minecraft:lightning_bolt",
    "minecraft:magic",
    "minecraft:on_fire",
    "minecraft:out_of_world",
    "minecraft:outside_border",
    "minecraft:stalagmite",
    "minecraft:starve",
    "minecraft:sweet_berry_bush",
    "minecraft:wither"
};

//#define REG_LENGTH_DIMENSION_TYPES 1
const char *registry_dimensionTypes[] = {
    "minecraft:overworld"
};

//#define REG_LENGTH_BIOME_TYPES 1
const char *registry_biomeTypes[] = {
    "minecraft:plains"
};

const char *registry_cat_variant[] = {
    "minecraft:white"
};
const char *registry_chicken_variant[] = {
    "minecraft:temperate"
};
const char *registry_cow_variant[] = {
    "minecraft:temperate"
};
const char *registry_frog_variant[] = {
    "minecraft:temperate"
};
const char *registry_painting_variant[] = {
    "minecraft:earth",
    "minecraft:wind",
    "minecraft:fire",
    "minecraft:water"
};
const char *registry_pig_variant[] = {
    "minecraft:temperate"
};
const char *registry_wolf_sound_variant[] = {
    "minecraft:classic"
};
const char *registry_wolf_variant[] = {
    "minecraft:black"
};
const char *registry_zombie_nautilus_variant[] = {
    "minecraft:temperate"
};
const char *registry_timeline[] = {
    "minecraft:day",
    "minecraft:early_game",
    "minecraft:moon"
};

int findRegId(size_t reg_length, void *registry, std::string id) {
    const char** reg = (const char **) registry;
    for (int i = 0; i < reg_length; i++) {
        if (std::string(reg[i]) == id) return i;
    }
}
