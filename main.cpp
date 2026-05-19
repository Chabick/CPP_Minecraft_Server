#include <iostream>

#include "Network/NetworkHandler.h"

//for utils
//#include <cjson/cJSON.h>

#include <thread>
#include <array>
#include <chrono>

//Custom Files
#include "Types/TShort.h"
#include "Types/TString.h"
#include "Types/TVarInt.h"
#include "Types/TShort.h"
#include "Util/tools.h"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "25565"

int __cdecl main() {
    NetworkHandler handler{};
    handler.engage();

    while (true) {
        handler.handleSafety();
        if (handler.status != ENGAGED) break;
    }

    return 0;
}

int __cdecl main_old() {

    WSADATA wsadata;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    //init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    //Create a SOCKET for the server tot listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //Accept a client socket
    printf("Waiting for connection...\n");
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //No longer need server socket
    closesocket(ListenSocket);

    //receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            ReadBufferInt packet_length = readVarInt(recvbuf, DEFAULT_BUFLEN, 0);
            ReadBufferInt packet_protocol = readVarInt(packet_length);
            ReadBufferInt protocol_version = readVarInt(packet_protocol);
            ReadBufferString server_address = readString(protocol_version);
            ReadBufferUShort server_port = readUShort(server_address);
            ReadBufferInt intent = readVarInt(server_port);

            printf("Packet length: %d\n", packet_length.result);
            printf("Protocol: %d\n", packet_protocol.result);
            printf("Protocol version: %d\n", protocol_version.result);
            printf("Server address: %s\n", server_address.result.c_str());
            printf("Server port: %d\n", server_port.result);
            printf("Intent: %d\n", intent.result);

            printf("------------------------------------------------------------");

            printf("Sending response");

            if (intent.result == 0x1) {
                char header[] = "{"
                    "\"version\":{\"name\":\"1.21.8\",\"protocol\":772},"
                    "\"description\":{\"text\":\"Hello World!\"},"
                    "\"players\": {\"max\":42690,\"online\": 0,\"sample\": []}";
                char footer[] = "}";

                uint16_t string_len = sizeof(header) + sizeof(footer) - 2;
                writeVarIntRaw(ClientSocket, 1 + string_len + sizeVarInt(string_len));
                writeByte(ClientSocket, 0x00);

                writeVarIntRaw(ClientSocket, string_len);
                send_all(ClientSocket, header, sizeof(header) - 1);
                send_all(ClientSocket, footer, sizeof(footer) - 1);

                /*
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()*/
            }
            break;

        } else if (iResult == 0)
            printf("Connection closed\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    //Shutdown of the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    //cleanup
    closesocket(ClientSocket);
    WSACleanup();


    return 0;
}
