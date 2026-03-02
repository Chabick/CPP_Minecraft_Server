#ifndef MCSERVER_LOCATION_H
#define MCSERVER_LOCATION_H
#include <cstdint>


namespace MC {
    struct ChunkColumnLocation {
        ChunkColumnLocation(int x, int y);
        int x;
        int y;

        bool operator==(const ChunkColumnLocation &loc) const;
    };

    struct ChunkLocation : public ChunkColumnLocation {
        ChunkLocation(int x, int y, uint8_t z);
        uint8_t z;

        bool operator==(const ChunkLocation &loc) const;
    };

    struct BlockLocation {
        BlockLocation(uint8_t x, uint8_t y, uint8_t z, ChunkLocation loc);
        BlockLocation(int x, int y, int z);

        uint8_t x, y, z;
        ChunkLocation chunk;

        bool operator==(const BlockLocation &loc) const;
    };

    ChunkColumnLocation getColumnLocFromBlock(BlockLocation* block);
    ChunkLocation getChunkLocFromBlock(BlockLocation* block);
}


#endif //MCSERVER_LOCATION_H