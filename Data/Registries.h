#ifndef MCSERVER_REGISTRIES_H
#define MCSERVER_REGISTRIES_H

#include <string>

#define REG_DAMAGE_TYPE 0
#define REG_DIMENSION_TYPE 1
#define REG_BIOME_TYPE 2

#define REG_LENGTH_DAMAGE_TYPES 25
#define REG_LENGTH_DIMENSION_TYPES 1
#define REG_LENGTH_BIOME_TYPES 1

#define REG_COUNT 12

extern int registry_lengths[];
extern const char *registries[];
extern void* registry_pointers[];
extern const char *registry_damageTypes[];
extern const char *registry_dimensionTypes[];
extern const char *registry_biomeTypes[];

extern const char *registry_cat_variant[];
extern const char *registry_chicken_variant[];
extern const char *registry_cow_variant[];
extern const char *registry_frog_variant[];
extern const char *registry_painting_variant[];
extern const char *registry_pig_variant[];
extern const char *registry_wolf_sound_variant[];
extern const char *registry_wolf_variant[];
extern const char *registry_zombie_nautilus_variant[];

extern const char *registry_timeline[];


int findRegId(size_t reg_length, void* registry, std::string id);

#endif //MCSERVER_REGISTRIES_H