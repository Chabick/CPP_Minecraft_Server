#ifndef MCSERVER_PACKETHANDLER_H
#define MCSERVER_PACKETHANDLER_H

#include "Util/tools.h"
#include "Types/TVarInt.h"
#include "Types/TString.h"

#include "Network/Client.h"
namespace PacketHandler {
    enum HandlerResponse {
        NONE, CLOSE
    };

    enum class PacketType {
        ADDPLAYER/*PlayerInfoUpdate*/,
    };
}

PacketHandler::HandlerResponse handlePacket(Client *client, char packetId, int length);
PacketHandler::HandlerResponse sendPacket(Client *client, PacketHandler::PacketType type);


#endif //MCSERVER_PACKETHANDLER_H