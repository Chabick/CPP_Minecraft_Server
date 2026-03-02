#ifndef MCSERVER_TOOLS_H
#define MCSERVER_TOOLS_H
#include <cstdint>

#include "ReadBuffer.h"
#include <winsock2.h>

ssize_t send_all(SOCKET socket, void* buffer, ssize_t len);
ssize_t recv_all (int client_fd, void *buf, size_t n, uint8_t require_first);
ssize_t writeByte(SOCKET socket, unsigned char byte);
char readByte(SOCKET socket);
void consume(SOCKET socket, size_t n);

void reverser(char* string, int length);


#endif //MCSERVER_TOOLS_H
