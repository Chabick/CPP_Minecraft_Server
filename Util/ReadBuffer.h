#ifndef MCSERVER_READBUFFER_H
#define MCSERVER_READBUFFER_H
#include <string>
#include <utility>


struct BufferReader {
    BufferReader(char* buffer, int size, int pointer)
        : buffer(buffer), size(size), pointer(pointer) {};

    char* buffer;
    int pointer;
    int size;
};

struct ReadBufferInt: BufferReader {
    ReadBufferInt(char* buffer, int size, int pointer, int result)
        : BufferReader(buffer, size, pointer), result(result) {};

    int result;
};

struct ReadBufferString: BufferReader {
    ReadBufferString(char* buffer, int size, int pointer, std::string result, int result_length)
        : BufferReader(buffer, size, pointer), result(std::move(result)), result_length(result_length) {};

    std::string result;
    int result_length;
};

struct ReadBufferShort: BufferReader {
    ReadBufferShort(char* buffer, int size, int pointer, short result)
        : BufferReader(buffer, size, pointer), result(result) {};

    short result;
};

struct ReadBufferUShort: BufferReader {
    ReadBufferUShort(char* buffer, int size, int pointer, unsigned short result)
        : BufferReader(buffer, size, pointer), result(result) {};

    unsigned short result;
};

#endif //MCSERVER_READBUFFER_H