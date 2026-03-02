#ifndef MCSERVER_WRITTENBUFFER_H
#define MCSERVER_WRITTENBUFFER_H


struct BufferWriter {
    BufferWriter(char* buffer, int size, int pointer)
        : buffer(buffer), size(size), pointer(pointer) {}

    char* buffer;
    int size;
    int pointer;
};


#endif //MCSERVER_WRITTENBUFFER_H