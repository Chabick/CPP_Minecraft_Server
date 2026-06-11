#ifndef MCSERVER_CLIENT_H
#define MCSERVER_CLIENT_H

#include <winsock2.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

#include "Minecraft/Minecraft.h"
#include "Util/Update.h"


struct Client {
    enum Status {
        NONE, HANDSHAKE, CONNECT, CLIENT_ERR, LOGIN, CONFIGURATION, PLAY
    };

    Client();
    Client(const Client& client);
    Client(SOCKET socket);
    ~Client();

    std::atomic<bool> hasGlobalUpdates;
    std::mutex globalUpdateMutex;
    std::queue<Update::TypedUpdate*> globalUpdates;

    std::atomic<bool> hasClientUpdates;
    std::mutex clientUpdatesMutex;
    std::queue<Update::TypedUpdate*> clientUpdates;

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