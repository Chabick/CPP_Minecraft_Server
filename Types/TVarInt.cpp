#include "TVarInt.h"

#include <cstdint>
#include <stdexcept>

#include "Util/tools.h"

#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80

ReadBufferInt readVarInt(char *buffer, int bufferSize, int startingPos) {
    int32_t value = 0;
    int position = 0;
    int pointer = startingPos;
    uint8_t currentChar;

    while (true) {
        currentChar = buffer[pointer++];

        //printf("current char: %d\n", currentChar);

        value |= ((currentChar & SEGMENT_BITS) << position);

        //printf("Value: %d\n", value);

        if ((currentChar & CONTINUE_BIT) == 0) break;
        position += 7;

        if (position >= 32) throw std::out_of_range("Error int readVarInt: found Integer bigger then 32 bytes");
    }

    return ReadBufferInt{buffer, bufferSize, pointer, value};
}

ReadBufferInt readVarInt(BufferReader reader) {
    int32_t value = 0;
    int position = 0;
    int pointer = reader.pointer;
    uint8_t currentChar;

    while (true) {
        currentChar = reader.buffer[pointer++];

        //printf("current char: %d\n", currentChar);

        value |= ((currentChar & SEGMENT_BITS) << position);

        //printf("Value: %d\n", value);

        if ((currentChar & CONTINUE_BIT) == 0) break;
        position += 7;

        if (position >= 32) throw std::out_of_range("Error int readVarInt: found Integer bigger then 32 bytes");
    }

    return ReadBufferInt{reader.buffer, reader.size, pointer, value};
}

int32_t readVarIntRaw(SOCKET socket) {
    int32_t value = 0;
    int position = 0;
    int pointer = 0;
    char currentChar;

    while (true) {
        currentChar = readByte(socket);

        //printf("current char: %d\n", currentChar);

        value |= ((currentChar & SEGMENT_BITS) << position);

        //printf("Value: %d\n", value);

        if ((currentChar & CONTINUE_BIT) == 0) break;
        position += 7;

        if (position >= 32) throw std::out_of_range("Error int readVarInt: found Integer bigger then 32 bytes");
    }

    return value;
}

BufferWriter writeVarInt(char* buffer, int size, int pointer, int32_t content) {
    while (true) {
        if ((content & ~SEGMENT_BITS) == 0) {
            buffer[pointer++] = content;
            return BufferWriter{buffer, size, pointer};
        }

        buffer[pointer++] = (content & SEGMENT_BITS) | CONTINUE_BIT;

        content >>= 7;
    }
}

void writeVarIntRaw(SOCKET client, int32_t content) {
    while (true) {
        if ((content & ~SEGMENT_BITS) == 0) {
            if (writeByte(client, content) == 0) throw std::runtime_error("ERROR writing to client");
            return;
        }

        if (writeByte(client, (content & SEGMENT_BITS) | CONTINUE_BIT) == 0) throw std::runtime_error("ERROR writing to client");

        content >>= 7;
    }
}

int sizeVarInt(uint32_t value) {
    int size = 1;
    while ((value & ~SEGMENT_BITS) != 0) {
        value >>= 7;
        size ++;
    }
    return size;
}