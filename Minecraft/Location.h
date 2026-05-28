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
        ChunkLocation chunk = ChunkLocation(0,0,0);

        bool operator==(const BlockLocation &loc) const;
    };

    //TODO: implement
    struct EntityLocation {
        EntityLocation(double x, double y, double z);
        EntityLocation(double x, double y, double z, double vx, double vy, double vz);
        EntityLocation(double x, double y, double z, double vx, double vy, double vz, float pitch, float yaw);

        double x, y, z;
        double vx, vy, vz;
        float pitch, yaw;

        bool operator|(const EntityLocation &loc) const; //will be for comparing just position
        bool operator|(const BlockLocation &loc) const; //check if pos is in block
        bool operator||(const BlockLocation &loc) const; //check if entity is completely in block (e.g. for gravity, checking for air under player)
        bool operator|(const ChunkLocation &loc) const; //check if pos is in chunk
        bool operator|(const ChunkColumnLocation &loc) const; //check if pos is in chunk column
        bool operator&(const EntityLocation &loc) const; //will be for comparing with pos&pitch/yaw
        bool operator==(const EntityLocation &loc) const; //will be for comparing everything


    };

    ChunkColumnLocation getColumnLocFromBlock(BlockLocation* block);
    ChunkLocation getChunkLocFromBlock(BlockLocation* block);
    ChunkLocation getChunkLocFromCoords(int x, int y, int z);
}


#endif //MCSERVER_LOCATION_H