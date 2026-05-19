#ifndef MCSERVER_NETSOCKET_H
#define MCSERVER_NETSOCKET_H

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__unix__)
#include <sys/socket.h>
#define SOCKET int
#endif


#endif //MCSERVER_NETSOCKET_H