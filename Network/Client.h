#ifndef MCSERVER_CLIENT_H
#define MCSERVER_CLIENT_H

#include <winsock2.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>

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
    std::thread *updateHandler;

    std::mutex updateHandlerMutex;
    std::condition_variable updateHandlerCondition;
    std::atomic<bool> hasPacket;


    bool isInternClosed() const {return internClosed;}

    //player_settings as long as the player is not implemented
    char view_distance = 16;
    std::shared_ptr<MC::Player> player;

    void createPlayer(uint8_t uuid[]);
private:
    bool internClosed = false;

    void handle();
    void handleUpdate();
    void _closeIntern();
};


#endif //MCSERVER_CLIENT_H