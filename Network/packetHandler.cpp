#include "Network/packetHandler.h"

#include "Data/Registries.h"
#include "Data/Tags.h"
#include "Types/TLong.h"
#include "Types/TShort.h"
#include "Minecraft/Minecraft.h"

PacketHandler::HandlerResponse handlePacket(Client *client, char packetId, int length) {
    if (client->status == Client::Status::PLAY) {
        //The communication during play
        //TODO: actually implement this
    } else if (client->status == Client::Status::NONE) {
        if (packetId != 0x00) return PacketHandler::CLOSE;

        int protocol_version = readVarIntRaw(client->socket);
        if (protocol_version != 774) {
            client->status = Client::Status::CLIENT_ERR;
            printf("[C] Client tried to connect with a wrong client version. Protocol Version: %d but 774 is required!", protocol_version);
            return PacketHandler::CLOSE;
        }

        std::string server_address = readStringRaw(client->socket);
        unsigned short server_port = readUShortRaw(client->socket);
        int intent = readVarIntRaw(client->socket);

        printf("[C] Handshake Handled with intent: %d to server: %s:%d\n", intent, server_address.c_str(), server_port);
        client->status = Client::Status::HANDSHAKE;

        if (intent == 0x01) {
            char header[] = "{"
                    "\"version\":{\"name\":\"1.21.8\",\"protocol\":774},"
                    "\"description\":{\"text\":\"Hello World!\"},"
                    "\"players\": {\"max\":42690,\"online\": 0,\"sample\": []}";
            char footer[] = "}";

            uint16_t string_len = sizeof(header) + sizeof(footer) - 2;
            writeVarIntRaw(client->socket, 1 + string_len + sizeVarInt(string_len));
            writeByte(client->socket, 0x00);

            writeVarIntRaw(client->socket, string_len);
            send_all(client->socket, header, sizeof(header) - 1);
            send_all(client->socket, footer, sizeof(footer) - 1);

            //flush the 0x01 0x00 packet
            readByte(client->socket);
            readByte(client->socket);

            //direct ping handling with an echo
            char *recvbuf_ping = (char *) malloc(10);
            for (int i=0; i<10; i++) {recvbuf_ping[i]=0x00;}
            recv_all(client->socket, recvbuf_ping, 10, false);
            send_all(client->socket, recvbuf_ping, 10);
            free(recvbuf_ping);

            return PacketHandler::CLOSE;
        } else if (intent == 0x02) {
            int packetLength = readVarIntRaw(client->socket);
            int packetId = readVarIntRaw(client->socket);
            if (packetId != 0x00) return PacketHandler::CLOSE;

            std::string playerName = readStringRaw(client->socket);

            uint8_t uuid[16]; //follows msb -> lsb
            int rcount = recv_all(client->socket, uuid, 16, false);
            if (rcount == -1) return PacketHandler::CLOSE;

            //TODO: maybe send actual player properties to display skins, refer to minecraft wiki mojang api calls for skin and cape
            writeVarIntRaw(client->socket, playerName.size() + sizeVarInt(playerName.size()) + 16 + 1 + sizeVarInt(0));
            writeByte(client->socket, 0x02);
            send_all(client->socket, uuid, 16);
            writeStringRaw(client->socket, playerName);
            //empty properties array
            writeVarIntRaw(client->socket, 0);

            //login acknowledged
            packetLength = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId != 0x03) return PacketHandler::CLOSE;
            client->status = Client::Status::CONFIGURATION;

            //send vanilla feature flag
            writeVarIntRaw(client->socket, 1 + sizeVarInt(1) + sizeVarInt(/*vanilla*/7) + 7);
            writeByte(client->socket, 0x0C);
            writeVarIntRaw(client->socket, 1);
            writeStringRaw(client->socket, "vanilla");

            //send known packs
            printf("[C] Synchronise known packs\n");
            char known_packs[] = {
                0x0e, 0x01, 0x09, 0x6d, 0x69, 0x6e,
                0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x04, 0x63,
                0x6f, 0x72, 0x65, 0x07, 0x31, 0x2e, 0x32, 0x31,
                0x2e, 0x31, 0x31
              };
            writeVarIntRaw(client->socket, 25);
            send_all(client->socket, &known_packs, 25);

            //recv client known packs
            known_packs_entry_point:
            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId == 0x02) {
                if (handlePacket(client, packetId, length) == PacketHandler::CLOSE) return PacketHandler::CLOSE;
                goto known_packs_entry_point;
            } else if (packetId == 0x00) {
                if (handlePacket(client, packetId, length) == PacketHandler::CLOSE) return PacketHandler::CLOSE;
                goto known_packs_entry_point;
            }
            if (packetId != 0x07) return PacketHandler::CLOSE;
            consume(client->socket, length-1); //ignore actual data


            printf("[C] Synchronise registry data\n");
            //send registry data
            for (int i = 0; i < REG_COUNT; i++) {
                const char *registryID = registries[i];
                int registryLength = registry_lengths[i];
                const char **registry = (const char **)registry_pointers[i];

                int packet_length = 1 + sizeStringRaw(std::string(registryID)) + sizeVarInt(registryLength) + registryLength;
                for (int e = 0; e < registryLength; e++) packet_length += sizeStringRaw(std::string(registry[e]));

                writeVarIntRaw(client->socket, packet_length); //size of packet
                writeByte(client->socket, 0x07); //packetID
                writeStringRaw(client->socket, std::string(registryID));
                writeVarIntRaw(client->socket, registryLength);

                for (int e = 0; e < registryLength; e++) {
                    const char *entry = registry[e];
                    writeStringRaw(client->socket, std::string(entry));
                    writeByte(client->socket, false); //no NBT
                }
            }

            //TODO: make a TagUpdate system and understand why and what tags to update
            /*int length = 1 + TAG_UPDATE_COUNT;
            for (int i = 0; i < TAG_UPDATE_COUNT; i++) {
                const char* registryID = update_registries[i];

                length += sizeStringRaw(std::string(registryID)) + ;
            }*/

            //TODO: do more research
            int length = 1 + 1 + sizeStringRaw("minecraft:timeline") + 1 + sizeStringRaw("minecraft:in_overworld") + sizeVarInt(1) + sizeVarInt(0);
            writeVarIntRaw(client->socket, length);
            writeByte(client->socket, 0x0D);
            writeVarIntRaw(client->socket, 1);
            writeStringRaw(client->socket, "minecraft:timeline");
            writeVarIntRaw(client->socket, 1);
            writeStringRaw(client->socket, "minecraft:in_overworld");
            writeVarIntRaw(client->socket, 1); //array length
            writeVarIntRaw(client->socket, 0); //num Id for binding


            printf("[C] Finished Configuration...");
            writeVarIntRaw(client->socket, 1);
            writeByte(client->socket, 0x03);

            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId != 0x03) {
                printf("declined -> canceling\n");
                return PacketHandler::CLOSE;
            }
            printf("Acknowledged\n");
            client->status = Client::Status::LOGIN;

            //Send Login(Play) packet

        }
    } else if (client->status == Client::Status::CONFIGURATION) {
        switch (packetId) {
            case 0x02: //Serverbound Plugin Message
                consume(client->socket, length-1); //ignore Packet
                break;
            case 0x00: //Client Information (configuration)
                std::string locale = readStringRaw(client->socket);
                client->view_distance = readByte(client->socket);

                consume(client->socket, length-1-locale.size()-sizeVarInt(locale.size())-1); //dont care for the rest
                break;

        }
    } else if (client->status == Client::Status::LOGIN) {
        switch (packetId) {

        }
    }

    return PacketHandler::NONE;
}
