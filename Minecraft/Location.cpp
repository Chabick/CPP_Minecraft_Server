#include "Location.h"

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
