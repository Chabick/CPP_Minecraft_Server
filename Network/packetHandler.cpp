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
#include "Types/TDouble.h"
#include "Util/Logger.h"
#include "Util/Update.h"

//look them up on the wayback machine on 1.1.2026
#define PACKET_ID_CHUNK_DATA_WITH_LIGHT 0x2C
#define PACKET_ID_GAME_EVENT 0x26
#define PACKET_ID_KEEP_ALIVE 0x2B

namespace RCVPID {//recv packet id
    inline constexpr char client_tick_end = 0x0c;
    inline constexpr char keep_alive = 0x1b;
    inline constexpr char player_move_position = 0x1d;
    inline constexpr char player_move_position_and_rotation = 0x1e;
}

PacketHandler::HandlerResponse handlePacket(Client *client, char packetId, int length) {
    if (client->status == Client::Status::PLAY) {
        //The communication during play
        switch (packetId) {
            case RCVPID::keep_alive:
                consume(client->socket, length-1);
                break;
            case RCVPID::client_tick_end:
                //TODO: actually implement
                consume(client->socket, length-1); //placeholder
                break;
            case RCVPID::player_move_position:
                //TODO: implement a cheat detection for movement
                client->player->location->x = readDoubleRaw(client->socket);
                client->player->location->y = readDoubleRaw(client->socket);
                client->player->location->z = readDoubleRaw(client->socket);
                consume(client->socket, 1); //consume the byte flag
            case RCVPID::player_move_position_and_rotation:
                client->player->location->x = readDoubleRaw(client->socket);
                client->player->location->x = readDoubleRaw(client->socket);
                client->player->location->x = readDoubleRaw(client->socket);
                client->player->location->yaw = readFloatRaw(client->socket);
                client->player->location->pitch = readFloatRaw(client->socket);
                consume(client->socket, 1); //consume the byte flag
            default:
                consume(client->socket, length-1);
        }
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
                    "\"version\":{\"name\":\"1.21.11\",\"protocol\":774},"
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
                    writeByte(client->socket, false); //no NBT //TODO: load nbt from data file
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
            plog.add((uint64_t) MC::SWorldConfig::hashed_seed);
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
            //increase scope for MutexLocks
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
            std::vector<Id> const * onlinePlayers = System::PlayerManagement::getPlayers();
            playerInfoUpdate.varInt(onlinePlayers->size());
            for (const auto& playerId : *onlinePlayers) { //updates also for the player itself
                //Add this player to the PacketData sent to the client in UpdatePlayerInfo
                System::PlayerManagement::PlayerT player = System::PlayerManagement::getPlayer(playerId);
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
        pUpdateTime.add((uint64_t) 0);
        pUpdateTime.add((uint64_t) 0);
        pUpdateTime.byte(false);
        pUpdateTime.send(client->socket);
        lgdb("Sent Packet-UpdateTime");

        //skipped packets: "Set Default Spawn Position"

        Packet pWaitForLevelChunks{};
        pWaitForLevelChunks.id(PACKET_ID_GAME_EVENT);
        pWaitForLevelChunks.byte(13);
        pWaitForLevelChunks.add((float) 0); //not important for this event
        pWaitForLevelChunks.send(client->socket);
        lgdb("Sent Packet-WaitForLevelChunks");

        //skipped packets: "Set Ticking State", "Step Tick"

        Packet pSetCenterChunk{};
        pSetCenterChunk.id(0x5C);
        pSetCenterChunk.varInt(0); //TODO: load player spawn pos and get chunk column x&y instead of 0:0
        pSetCenterChunk.varInt(0);
        pSetCenterChunk.send(client->socket);
        lgdb("Sent Packet-SetCenterChunks");

        std::vector<std::pair<int, int>> coords{};
        coords.reserve(9);
        coords.emplace_back(0, 0);
        coords.emplace_back(0, 1);
        coords.emplace_back(1, 1);
        coords.emplace_back(1, 0);
        coords.emplace_back(1, -1);
        coords.emplace_back(0, -1);
        coords.emplace_back(-1, -1);
        coords.emplace_back(-1, 0);
        coords.emplace_back(-1, 1);

        for (int i = 0; i < 9; i++) {
            Packet pChunkData{};
            pChunkData.id(PACKET_ID_CHUNK_DATA_WITH_LIGHT);
            pChunkData.add(coords.at(i).first);//(int) 0); //Chunk x
            pChunkData.add(coords.at(i).second);//(int) 0); //Chunk y
            pChunkData.varInt(0); //no heightmap

            //for VarInt Size (for size of data field)
            Packet::Counter* c = pChunkData.add(Packet::Counter::VarInt);

            //first ChunkSection
            pChunkData.add(static_cast<short>(1));
            c->up(2);
            pChunkData.byte(4);
            c->up();
            pChunkData.varInt(2); //length of prefixed array
            c->up(sizeVarInt(2));
            pChunkData.varInt(0); //air
            c->up(sizeVarInt(0));
            pChunkData.varInt(1); //stone
            c->up(sizeVarInt(1));

            pChunkData.add(static_cast<uint64_t>(0x0000000000000001));//0x8000000000000000); //at x,y,z=0 a stone block
            c->up(8);
            for (int i = 0; i < 255; i++) {
                pChunkData.add(static_cast<uint64_t>(0));
                c->up(8);
            }
            //biomes in 4x4x4 sections
            pChunkData.byte(0);
            c->up();
            pChunkData.varInt(0);
            c->up();

            //Empty ChunkSections
            for (int i = 0; i < 23; i++) {
                pChunkData.add(static_cast<short>(0));
                c->up(2);

                //empty blocks and biomes
                pChunkData.byte(0);
                c->up();
                pChunkData.varInt(0);
                c->up();

                pChunkData.byte(0);
                c->up();
                pChunkData.varInt(0);
                c->up();
            };

            //no block entities
            pChunkData.varInt(0);

            //no light
            pChunkData.varInt(0);
            pChunkData.varInt(0);
            pChunkData.varInt(1);
            pChunkData.add(static_cast<uint64_t>(0x03ffffff));
            pChunkData.varInt(1);
            pChunkData.add(static_cast<uint64_t>(0x03ffffff));
            pChunkData.varInt(0);
            pChunkData.varInt(0);

            pChunkData.build(); //inserts all Counters with shifting
            pChunkData.send(client->socket);
            lgdb("Sent Packet-ChunkData");
        }

        Packet pKeepAlive{};
        pKeepAlive.id(PACKET_ID_KEEP_ALIVE);
        pKeepAlive.add((uint64_t) 0);
        pKeepAlive.send(client->socket);
        lgdb("Sent Packet-KeepAlive");

        client->status = Client::Status::PLAY;
        info("Client Connected");


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