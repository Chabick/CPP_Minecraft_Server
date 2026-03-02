#ifndef MCSERVER_TLONG_H
#define MCSERVER_TLONG_H

#include <cstdint>
#include <winsock2.h>

bool checkLittleEndian();

void writeLongRaw(SOCKET socket, int64_t data);
int64_t readLongRaw(SOCKET socket); //TODO: this function has some kind of error!
char* readLong(SOCKET socket);


#endif //MCSERVER_TLONG_H