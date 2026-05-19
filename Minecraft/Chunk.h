#ifndef MCSERVER_CHUNK_H
#define MCSERVER_CHUNK_H

#include <vector>
#include <map>
#include <cstdint>

namespace MC {
    class Chunk {

    };

    class ChunkColumn {
        std::vector<MC::Chunk> chunks;
        std::map<uint8_t, Chunk*> p_chunks;
        std::vector<uint16_t> block_types;
    };
}

#endif //MCSERVER_CHUNK_H