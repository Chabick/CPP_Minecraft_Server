#include "NetworkHandler.h"

#include <cstdio>
#include <iostream>

//TODO: add the linux socket init version

NetworkHandler::NetworkHandler() {
    clients = std::vector<std::shared_ptr<Client>>();
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

        while (hasClient) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        clientsQueue.push(std::make_shared<Client>(cl));
        hasClient = true;
        free(addr);
        free(addrlen);
    }
}

void NetworkHandler::handleSafety() {
    try {
        if (hasClient) {
            while (!clientsQueue.empty()) {
                clients.push_back(clientsQueue.front());
                clientsQueue.pop();
            }
            hasClient = false;
        }

        if (!clients.empty()) {
            auto it = clients.begin();
            while (it != clients.end()) {
                if (it->get()->isInternClosed()) {
                    it = clients.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    } catch (std::exception &e) {
        std::cout << "Exception in safety handling!" << std::endl;
        std::cout << e.what();
    }
}

NetworkHandler::~NetworkHandler() {}

void NetworkHandler::handleDistributions() {}

void NetworkHandler::handleUpdates() {
    for (std::shared_ptr<Client> client : clients) {
        if (client->hasGlobalUpdates) {
            std::scoped_lock<std::mutex> lock{client->globalUpdateMutex};
            while (!client->globalUpdates.empty()) {
                auto update = client->globalUpdates.front();

                switch (update->type) {
                    case Update::ADDPLAYER:
                        auto data = update->addPlayer;
                        for (const std::shared_ptr<Client>& cl : clients) {
                            if (&(*client) != &(*cl)) {
                                cl->clientUpdatesMutex.lock();
                                cl->clientUpdates.push(update);
                                cl->clientUpdatesMutex.unlock();
                                cl->hasClientUpdates = true;
                                cl->updateHandlerCondition.notify_one();
                            }
                        }
                        break;
                }

                client->globalUpdates.pop();
            }
        }
    }
}

