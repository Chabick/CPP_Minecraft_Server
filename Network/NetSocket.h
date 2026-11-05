#ifndef MCSERVER_NETSOCKET_H
#define MCSERVER_NETSOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#elif defined(__unix__)
#include <sys/socket.h>
#define SOCKET int
#endif


#endif //MCSERVER_NETSOCKET_H