#include "Network/Client.h"

#include "packetHandler.h"
#include "Types/TVarInt.h"

Client::Client()
    : socket(INVALID_SOCKET), status(Client::Status::CLIENT_ERR) {}

Client::Client(const Client &client) {
    this->socket = client.socket;
    this->status = client.status;
    //TODO: Maybe causes an error where the client handler thread disappears
    this->handler = std::move(client.handler);
}

Client::Client(SOCKET socket)
 : socket(socket), status(Client::Status::NONE) {
    handler = new std::thread(&Client::handle, this);
}

Client::~Client() {
    //TODO: check for disconnect, then fr
}

void Client::operator=(const Client &client) {
    this->socket = client.socket;
    this->status = client.status;
    this->handler = std::move(client.handler);
}

void Client::handle() {
    printf("[C] Connection Handled\n");
    while (true) {
        try {
            int length = readVarIntRaw(this->socket);
            int packet_id = readVarIntRaw(this->socket);
            PacketHandler::HandlerResponse response = handlePacket(this, packet_id, length);
            if (response == PacketHandler::CLOSE) {
                this->_closeIntern();
                return;
            }
        } catch (std::runtime_error &e) {
            //TODO: disconnect or coordinate with destructor
            printf("[E] An error occurred while trying to handle packet!\n");
            printf(e.what());
            printf("\n");

            this->_closeIntern();
            break;
        }
    }
    printf("[C] Connection broken\n");
}

void Client::_closeIntern() {
    //TODO: do additional closing stuff
    shutdown(socket, SD_BOTH);
    internClosed = true;
    printf("[C] Connection Closed\n");
}
