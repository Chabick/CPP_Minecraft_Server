#ifndef MCSERVER_TSTRING_H
#define MCSERVER_TSTRING_H

#include <string>
#include <winsock2.h>

#include "Util/ReadBuffer.h"


ReadBufferString readString(char* buffer, int bufferSize, int StartingPos);
ReadBufferString readString(BufferReader reader);
std::string readStringRaw(SOCKET socket);
void writeStringRaw(SOCKET socket, std::string str);

int sizeStringRaw(std::string str);


#endif //MCSERVER_TSTRING_H