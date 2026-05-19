#ifndef MCSERVER_CLIENT_H
#define MCSERVER_CLIENT_H

#include <winsock2.h>
#include <thread>
#include "Minecraft/Minecraft.h"


struct Client {
    enum Status {
        NONE, HANDSHAKE, CONNECT, CLIENT_ERR, LOGIN, CONFIGURATION, PLAY
    };

    Client();
    Client(const Client& client);
    Client(SOCKET socket);
    ~Client();

    void operator=(const Client& client);

    SOCKET socket;
    Client::Status status;
    int pinged = 0;

    std::thread* getHandlerCopy(Client client)
        {return client.handler;};

    std::thread *handler;

    bool isInternClosed() const {return internClosed;}

    //player_settings as long as the player is not implemented
    char view_distance = 16;
    MC::Player* player = nullptr;

    void createPlayer();
private:
    bool internClosed = false;

    void handle();
    void _closeIntern();
};


#endif //MCSERVER_CLIENT_H