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
