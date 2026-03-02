#ifndef MCSERVER_TUSHORT_H
#define MCSERVER_TUSHORT_H

#include "Util/ReadBuffer.h"
#include <winsock2.h>

ReadBufferShort readShort(char* buffer, int size, int pointer);
ReadBufferShort readShort(BufferReader reader);
short readShortRaw(SOCKET socket);

ReadBufferUShort readUShort(char* buffer, int size, int pointer);
ReadBufferUShort readUShort(BufferReader reader);
unsigned short readUShortRaw(SOCKET socket);


#endif //MCSERVER_TUSHORT_H
