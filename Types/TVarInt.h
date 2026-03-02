#ifndef MCSERVER_VARINT_H
#define MCSERVER_VARINT_H

#include <cstdint>
#include <winsock2.h>

#include "Util/ReadBuffer.h"
#include "Util/WrittenBuffer.h"

ReadBufferInt readVarInt(char* buffer, int bufferSize, int StartingPos);
ReadBufferInt readVarInt(BufferReader reader);
int32_t readVarIntRaw(SOCKET socket);

BufferWriter writeVarInt(char* buffer, int size, int StartingPos, int32_t content);
void writeVarIntRaw(SOCKET socket, int32_t content);

int sizeVarInt(uint32_t content);


#endif //MCSERVER_VARINT_H