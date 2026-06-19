#include "Network/packetHandler.h"

#include <iostream>
#include <memory>

#include "Packet.h"
#include "Data/Registries.h"
#include "Data/Tags.h"
#include "Types/TLong.h"
#include "Types/TShort.h"
#include "Types/TPrefixedArray.h"
#include "Minecraft/Minecraft.h"
#include "Systems/PlayerManagement.h"
#include "Util/Update.h"


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
            if (packetId == 0x02) { //handle "Serverbound Plugin Message"
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


            //completed: do more research
            //maybe I understood it? idk
            /*
            int length = 1 + 1 + sizeStringRaw("minecraft:timeline") + 1 + sizeStringRaw("minecraft:in_overworld") + sizeVarInt(1) + sizeVarInt(0);
            writeVarIntRaw(client->socket, length);
            writeByte(client->socket, 0x0D);
            writeVarIntRaw(client->socket, 1);
            writeStringRaw(client->socket, "minecraft:timeline");
            writeVarIntRaw(client->socket, 1);
            writeStringRaw(client->socket, "minecraft:in_overworld");
            writeVarIntRaw(client->socket, 1); //array length
            writeVarIntRaw(client->socket, 0); //num Id for binding*/

            //send update tags packet
            Packet pUpdateTags{};
            pUpdateTags.id(0x0D);
            pUpdateTags.varInt(GlobalTags.size());
            for (auto tags : GlobalTags) {
                pUpdateTags.string(tags.registry_id);
                pUpdateTags.varInt(tags.tags.size());
                for (auto tag : tags.tags) {
                    pUpdateTags.string(tag.identifier);
                    pUpdateTags.prefixedArray(tag.binding_ids);
                }
            }
            pUpdateTags.send(client->socket);
            /*writeVarIntRaw(client->socket, PACKET_UPDATE_TAGS_LENGTH);
            writeByte(client->socket, 0x0D);
            writeVarIntRaw(client->socket, GlobalTags.size());
            for (auto tags : GlobalTags) {
                writeStringRaw(client->socket, tags.registry_id);
                writeVarIntRaw(client->socket, tags.tags.size());
                for (auto tag : tags.tags) {
                    writeStringRaw(client->socket, tag.identifier);
                    writePrefixedArrayRaw(client->socket, tag.binding_ids); //automatically writes the length
                }
            }*/


            printf("[C] Finished Configuration...");
            Packet pFinishedConfig{};
            pFinishedConfig.id(0x03);
            pFinishedConfig.send(client->socket);

            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId != 0x03) {
                printf("declined -> canceling\n");
                return PacketHandler::CLOSE;
            }
            printf("Acknowledged\n");
            client->status = Client::Status::LOGIN;



            //at this point the client has a player
            client->createPlayer(uuid);
            client->player->name = playerName;


            //Send Login(Play) packet
            Packet plog;
            plog.id(0x30);
            plog.add((int)client->player->getEID());
            plog.byte(MC::SWorldConfig::is_hardcore);
            plog.varInt(MC::SWorldConfig::dimension_ids.size());
            for (int e = 0; e < MC::SWorldConfig::dimension_ids.size(); e++) {
                plog.string(MC::SWorldConfig::dimension_ids[e]);
            }
            plog.varInt(42690);
            plog.varInt(MC::SWorldConfig::view_distance);
            plog.varInt(MC::SWorldConfig::simulation_distance);
            plog.byte(MC::SWorldConfig::reduced_debug_info);
            plog.byte(MC::SWorldConfig::enable_respawn_screen);
            plog.byte(MC::SWorldConfig::do_limited_crafting);
            plog.varInt(MC::SWorldConfig::spawndim_num_id);
            plog.string(MC::SWorldConfig::spawndim_id);
            plog.add((long) MC::SWorldConfig::hashed_seed);
            plog.byte(client->player->game_mode);
            plog.byte(client->player->prev_game_mode);
            plog.byte(MC::SWorldConfig::is_debug);
            plog.byte(MC::SWorldConfig::is_flat);
            plog.byte(client->player->has_died);
            plog.varInt(client->player->portal_cooldown);
            plog.varInt(MC::SWorldConfig::world->sea_level);
            plog.byte(false); //no secure chat*/

            plog.send(client->socket);
            std::cout << "[C] Client logged in" << std::endl;

            Packet diff_change;
            diff_change.id(0x0A);
            diff_change.byte(MC::SWorldConfig::difficulty);
            diff_change.byte(false); //diff is not locked
            diff_change.send(client->socket);

            //possible, but for now skipped, packets: "Player Abilities", "Set Held Item",
            //  "Update Recipes", "Entity Event", "Commands", "Update Recipe Book"

            Packet ppos;
            ppos.id(0x46);
            ppos.varInt(42); //random tp id
            ppos.add((double)client->player->location->x);
            ppos.add((double)client->player->location->y);
            ppos.add((double)client->player->location->z);
            ppos.add((double)client->player->location->vx);
            ppos.add((double)client->player->location->vy);
            ppos.add((double)client->player->location->vz);
            ppos.add((float)client->player->location->yaw);
            ppos.add((float)client->player->location->pitch);
            ppos.add((int)0);

            ppos.send(client->socket);


            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId == 0x15) {
                consume(client->socket, length-1);
            }
            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId != 0x00) {
                printf("[C] Teleport of Joining Player failed!\n");
                return PacketHandler::HandlerResponse::CLOSE;
            }
            //check the tp id
            if (readVarIntRaw(client->socket) != 42) {
                printf("[C] Teleport of Joining Player wasn't accepted by client -> Canceling connection\n");
                return PacketHandler::HandlerResponse::CLOSE;
            }

            length = readVarIntRaw(client->socket);
            packetId = readVarIntRaw(client->socket);
            if (packetId == 0x1e) { //consume "Set Player Position and Rotation"
                consume(client->socket, length-1);
            }

            //possible, but for now skipped, packets: "Server Data"

            //create the Player Joining Update
            //increase scope
            if constexpr (true) {
                //add the Player Joining to the GlobalUpdate Que
                client->globalUpdateMutex.lock();
                Update::Data::_GlobalAddPlayer data{client->player->name.c_str()};
                auto* update = new Update::TypedUpdate();
                update->type = Update::ADDPLAYER;
                update->addPlayer = data;
                client->globalUpdates.push(update);
                client->globalUpdateMutex.unlock();
                client->hasGlobalUpdates = true;
            }

            //get all other players currently online and update their info
            Packet playerInfoUpdate{};
            playerInfoUpdate.id(0x44);
            playerInfoUpdate.byte(0x01);
            std::shared_ptr<const std::vector<std::shared_ptr<MC::Player>>> onlinePlayers = System::PlayerManagement::getPlayers();
            playerInfoUpdate.varInt(onlinePlayers->size());
            for (const auto& player : *onlinePlayers) { //updates also for the player itself
                //Add this player to the PacketData send to the client in UpdatePlayerInfo
                for (int i = 0; i < 16; i++) {
                    playerInfoUpdate.byte(player->getUUId()[i]);
                }
                playerInfoUpdate.string(player->name);
                playerInfoUpdate.varInt(0); //send no skin or cape data to the client
            }
            playerInfoUpdate.send(client->socket);
        }

        //skipped packets: "Initialize World Border"

        Packet pUpdateTime{};
        pUpdateTime.id(0x6f);
        pUpdateTime.add((long) 0);
        pUpdateTime.add((long) 0);
        pUpdateTime.byte(false);
        pUpdateTime.send(client->socket);

        //skipped packets: "Set Default Spawn Position"

        Packet pWaitForLevelChunks{};
        pWaitForLevelChunks.id(0x26);
        pWaitForLevelChunks.byte(13);
        pWaitForLevelChunks.add((float) 0); //not important for this event

        //skipped packets: "Set Ticking State", "Step Tick"

        Packet pSetCenterChunk{};



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
    } else if (client->status == Client::Status::LOGIN) { //currently unused because of different send orders
        switch (packetId) {}
    }

    return PacketHandler::NONE;
}

PacketHandler::HandlerResponse sendPacket(Client *client, PacketHandler::PacketType packetType) {
    switch (packetType) {
        case PacketHandler::PacketType::ADDPLAYER:
            Packet packet{};
            packet.id(0x44);
            packet.byte(0x01);
            packet.varInt(1);
            for (int i = 0; i < 8; i++) {
                packet.byte(client->player->getUUId()[i]);
            }
            packet.string(client->player->name);
            packet.varInt(0); //send no skin and cape data
            packet.send(client->socket);
            break;
    }
}