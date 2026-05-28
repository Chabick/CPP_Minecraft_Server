#include "Location.h"

#include <math.h>

MC::ChunkColumnLocation::ChunkColumnLocation(int x, int y)
    : x(x), y(y) {}

bool MC::ChunkColumnLocation::operator==(const ChunkColumnLocation &loc) const {
    return loc.x == x && loc.y == y;
}

MC::ChunkLocation::ChunkLocation(int x, int y, uint8_t z)
    : ChunkColumnLocation(x, y), z(z) {}

bool MC::ChunkLocation::operator==(const ChunkLocation &loc) const {
    return x == loc.x && y == loc.y && z == loc.z;
}

MC::ChunkColumnLocation MC::getColumnLocFromBlock(BlockLocation* block) {
    int x = (block->x)/16;
    int y = (block->y)/16;
    return MC::ChunkColumnLocation(x, y);
}

MC::ChunkLocation MC::getChunkLocFromBlock(BlockLocation *block) {
    int x = (block->x)/16;
    int y = (block->y)/16;
    int z = (block->z)/16;
    return MC::ChunkLocation(x, y, z);
}

MC::ChunkLocation MC::getChunkLocFromCoords(int x, int y, int z) {
    int cx = x / 16;
    int cy = y / 16;
    uint8_t cz = z / 16;

    return {cx, cy, cz};
}

MC::BlockLocation::BlockLocation(uint8_t x, uint8_t y, uint8_t z, ChunkLocation loc)
    : x(x), y(y), z(z), chunk(loc) {}

MC::BlockLocation::BlockLocation(int x, int y, int z) {
    int cx = x / 16;
    int cy = y / 16;
    int cz = z / 16;

    int ix = x - cx * 16;
    int iy = y - cy * 16;
    int iz = z - cx * 16;

    this->x = ix;
    this->y = iy;
    this->z = iz;

    this->chunk = MC::ChunkLocation(cx, cy, cz);
}

bool MC::BlockLocation::operator==(const BlockLocation &loc) const {
    return x == loc.x && y == loc.y && z == loc.z && chunk == loc.chunk;
}

MC::EntityLocation::EntityLocation(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;

    this->vx = 0;
    this->vy = 0;
    this->vz = 0;

    this->pitch = 0;
    this->yaw = 0;
}

MC::EntityLocation::EntityLocation(double x, double y, double z, double vx, double vy, double vz) {
    this->x = x;
    this->y = y;
    this->z = z;

    this->vx = vx;
    this->vy = vy;
    this->vz = vz;

    this->pitch = 0;
    this->yaw = 0;
}

MC::EntityLocation::EntityLocation(double x, double y, double z, double vx, double vy,
    double vz, float pitch, float yaw) {
    this->x = x;
    this->y = y;
    this->z = z;

    this->vx = vx;
    this->vy = vy;
    this->vz = vz;

    this->pitch = pitch;
    this->yaw = yaw;
}

bool MC::EntityLocation::operator|(const EntityLocation &loc) const {
    return this->x == loc.x && this->y == loc.y && this->z == loc.z;
}

bool MC::EntityLocation::operator|(const BlockLocation &loc) const {
    return std::trunc(this->x) == loc.x && std::trunc(this->y) == loc.y
    && std::trunc(this->z) == loc.z;
}

bool MC::EntityLocation::operator||(const BlockLocation &loc) const {
    return (*this | loc); //TODO: implement proper test for being inside block boundaries
}

bool MC::EntityLocation::operator|(const ChunkLocation &loc) const {
    return MC::getChunkLocFromCoords(std::trunc(x), std::trunc(y), std::trunc(z)) == loc;
}

bool MC::EntityLocation::operator|(const ChunkColumnLocation &loc) const {
    return ((MC::ChunkColumnLocation)
        MC::getChunkLocFromCoords(
            std::trunc(x), std::trunc(y), std::trunc(z))) == loc;
}

bool MC::EntityLocation::operator&(const MC::EntityLocation &loc) const {
    return this->x == loc.x && this->y == loc.y && this->z == loc.z &&
        this->pitch == loc.pitch && this->yaw == loc.yaw;
}

bool MC::EntityLocation::operator==(const EntityLocation &loc) const {
    return (*this & loc) && this->vx == loc.vx && this->vy == loc.vy && this->vz == loc.vz;
}
