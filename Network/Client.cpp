#include "Network/Client.h"

#include "packetHandler.h"
#include "Types/TVarInt.h"

#include <chrono>

#include "Systems/PlayerManagement.h"

Client::Client()
    : socket(INVALID_SOCKET), status(Client::Status::CLIENT_ERR) {}

Client::Client(const Client &client) {
    this->socket = client.socket;
    this->status = client.status;
    //TODO: Maybe causes an error where the client handler thread disappears
    this->handler = std::move(client.handler);
    this->updateHandler = std::move(client.updateHandler);
}

Client::Client(SOCKET socket)
 : socket(socket), status(Client::Status::NONE) {
    handler = new std::thread(&Client::handle, this);
    updateHandler = new std::thread(&Client::handleUpdate, this);
}

Client::~Client() {
    //TODO: check for disconnect, then fr
    //player already deleted
    if (handler->joinable()) handler->join();
    if (updateHandler->joinable()) updateHandler->join();

    delete handler;
    delete updateHandler;
}

void Client::operator=(const Client &client) {
    this->socket = client.socket;
    this->status = client.status;
    this->handler = std::move(client.handler);
    this->updateHandler = std::move(client.updateHandler);
}

void Client::handleUpdate() {
    try {
        while (!this->internClosed) {
            std::unique_lock<std::mutex> lock(updateHandlerMutex);
            updateHandlerCondition.wait(lock, [this]() {return this->hasClientUpdates == true || this->isInternClosed();}); //needs the check bc atomic
            if (this->isInternClosed()) return;
            while (this->hasPacket) {std::this_thread::sleep_for(std::chrono::nanoseconds(5));}
            SuspendThread(reinterpret_cast<HANDLE>(this->handler->native_handle()));

            //handle update
            clientUpdatesMutex.lock();
            while (!this->clientUpdates.empty()) {
                Update::TypedUpdate* update = this->clientUpdates.front();
                this->clientUpdates.pop();
                switch (update->type) {
                    case Update::Type::ADDPLAYER:
                        auto response = sendPacket(this, PacketHandler::PacketType::ADDPLAYER);
                        if (response == PacketHandler::CLOSE) {
                            this->internClosed = true;
                            this->handler->join();
                            return;
                        }
                        break;
                }
                delete update;
            }
            clientUpdatesMutex.unlock();

            ResumeThread(reinterpret_cast<HANDLE>(this->handler->native_handle()));
        }
    } catch (const std::exception &e) {
        fprintf(stderr, "HANDLEUPDATE: %s\n", e.what());
    }
}

void Client::handle() {
    printf("[C] Connection Handled\n");
    while (!this->internClosed) {
        try {
            int length = readVarIntRaw(this->socket);
            this->hasPacket = true; //pause client update handler
            int packet_id = readVarIntRaw(this->socket);
            PacketHandler::HandlerResponse response = handlePacket(this, packet_id, length);
            if (response == PacketHandler::CLOSE) {
                this->_closeIntern();
                return;
            }
            this->hasPacket = false;
        } catch (std::runtime_error &e) {
            //TODO: disconnect or coordinate with destructor
            printf("[E] An error occurred while trying to handle packet!\n");
            printf(e.what());
            printf("\n");

            this->_closeIntern();
            break;
        } catch (std::exception &e) {
            printf("[E] A critical error occurred, that couldn't be handled!\n");
            printf(e.what());
            printf("\nResolving by shutting down!\n");

            this->_closeIntern();
            break;
        }
    }
    printf("[C] Connection broken\n");
}

void Client::_closeIntern() {
    //TODO: do additional closing stuff
    shutdown(socket, SD_BOTH);
    closesocket(socket);
    internClosed = true;
    updateHandlerCondition.notify_one();
    printf("[C] Connection Closed\n");
    System::PlayerManagement::unregisterPlayer(player);
}

void Client::createPlayer(uint8_t uuid[]) {
    this->player = std::make_shared<MC::Player>(uuid, this);
    System::PlayerManagement::registerPlayer(this->player);
}
