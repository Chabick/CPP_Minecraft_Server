#ifndef MCSERVER_NETWORKHANDLER_H
#define MCSERVER_NETWORKHANDLER_H

//for server
#include <algorithm>
#include "NetWork/NetSocket.h"

#include <thread>
#include <vector>
#include <queue>

#include "Client.h"

#define DEFAULT_BUFLEN 1024
#define NETWORK_PORT "25565"
#define NETWORK_ADDR "127.0.0.1"

enum NetworkStatus {
    ENGAGED, INIT, ERR_STARTUP, ERR_ADDRINFO, ERR_NEWSOCK, ERR_BIND, ERR_LISTEN
};

class NetworkHandler {

    std::thread networkThread;
    std::vector<Client> clients;
    std::queue<Client> clientsAddQueue;
    std::queue<Client> clientsRemQueue;

    SOCKET ListenSocket;
    WSADATA wsadata;
    int iResult;
    struct addrinfo *result;
    struct addrinfo hints;

    void _engage();

public:
    NetworkStatus status;

    NetworkHandler();
    ~NetworkHandler();

    void engage();
    void handleSafety();
};


#endif //MCSERVER_NETWORKHANDLER_H