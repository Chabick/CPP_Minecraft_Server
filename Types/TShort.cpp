#include "TShort.h"

#include "Util/tools.h"

ReadBufferShort readShort(char* buffer, int size, int pointer) {
    short result = (buffer[pointer] << 8) | (buffer[pointer + 1] & 0xFF);
    pointer += 2;
    return ReadBufferShort{buffer, size, pointer, result};
}

ReadBufferShort readShort(BufferReader reader) {
    short result = (reader.buffer[reader.pointer] << 8) | (reader.buffer[reader.pointer + 1] & 0xFF);
    reader.pointer += 2;
    return ReadBufferShort{reader.buffer, reader.size, reader.pointer, result};
}

ReadBufferUShort readUShort(char* buffer, int size, int pointer) {
    unsigned short result = (buffer[pointer] << 8) | (buffer[pointer + 1] & 0xFF);
    pointer += 2;
    return ReadBufferUShort{buffer, size, pointer, result};
}

ReadBufferUShort readUShort(BufferReader reader) {
    unsigned short result = (reader.buffer[reader.pointer] << 8) | (reader.buffer[reader.pointer + 1] & 0xFF);
    reader.pointer += 2;
    return ReadBufferUShort{reader.buffer, reader.size, reader.pointer, result};
}

short readShortRaw(SOCKET socket) {
    char msb = readByte(socket);
    char lsb = readByte(socket);
    short result = (msb << 8) | (lsb & 0xFF);
    return result;
}

unsigned short readUShortRaw(SOCKET socket) {
    char msb = readByte(socket);
    char lsb = readByte(socket);
    unsigned short result = (msb << 8) | (lsb & 0xFF);
    return result;
}