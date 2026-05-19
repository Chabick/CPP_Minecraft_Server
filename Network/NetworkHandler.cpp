#include "NetworkHandler.h"

#include <cstdio>

//TODO: add the linux socket init version

NetworkHandler::NetworkHandler() {
    clients = std::vector<Client>();
    clientsAddQueue = std::queue<Client>();
    clientsRemQueue = std::queue<Client>();
    ListenSocket = INVALID_SOCKET;

    //Init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        status = ERR_STARTUP;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolve the server address and port
    iResult = getaddrinfo(NULL, NETWORK_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        status = ERR_ADDRINFO;
        return;
    }

    //Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        status = ERR_NEWSOCK;
        return;
    }

    //Setup the TCP listen socket
    iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        status = ERR_BIND;
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        status = ERR_LISTEN;
        return;
    }

    status = INIT;
}

void NetworkHandler::engage() {
    networkThread = std::thread(&NetworkHandler::_engage, this);

    status = ENGAGED;
}


void NetworkHandler::_engage() {
    printf("[I] Trying to engage\n");
    while (true) {
        //Accept a new client connection
        sockaddr *addr = nullptr;
        int *addrlen = nullptr;
        SOCKET cl = accept(ListenSocket, addr, addrlen);
        /*if (addr == nullptr || addrlen == nullptr) {
            printf("[W] Connection Address or AddressLen is NULL. Might result in error.");
        }*/
        printf("[C] New Connection from %d\n", addr);
        if (cl == INVALID_SOCKET) {
            printf("[E] Connection accept failed with error: %d\n", WSAGetLastError());
        }

        Client client{cl};

        clientsAddQueue.push(client);
    }
}

void NetworkHandler::handleSafety() {
    while (clientsAddQueue.size() > 0) {
        clients.push_back(clientsAddQueue.front());
        clientsAddQueue.pop();
    }

    for (Client client : clients) {
        if (client.isInternClosed()) clientsRemQueue.push(client);
    }

    while (clientsRemQueue.size() > 0) {
        Client client = clientsRemQueue.front();
        for (int i = 0; i < clients.size(); i++) {
            if (clients.at(i).socket == client.socket) {
                clients.erase(clients.cbegin() + i);
                break;
            }
        }
        clientsRemQueue.pop();
    }


}

NetworkHandler::~NetworkHandler() {}
