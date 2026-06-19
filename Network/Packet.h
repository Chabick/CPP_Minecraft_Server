#ifndef MCSERVER_PACKET_H
#define MCSERVER_PACKET_H

#include <cstdint>
#include <string>
#include <vector>

#include "winsock2.h"

class Packet {
    std::vector<char> buff;
    char _id = 0x00;

public:
    void add(int a);
    void add(bool a);
    void add(char a);
    void add(short a);
    void add(long a);
    void add(float a);
    void add(double a);
    void add(std::string a);

    void varInt(int32_t a);
    void string(const char* a);
    void string(std::string a);
    void byte(unsigned char a);

    void prefixedArray(std::vector<int> a);

    void id(char a) {_id = a;}

    void send(SOCKET socket);
};


#endif //MCSERVER_PACKET_H