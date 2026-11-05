#ifndef MCSERVER_PACKET_H
#define MCSERVER_PACKET_H

#include <cstdint>
#include <string>
#include <vector>

#include "winsock2.h"

class Packet {
public:
    struct Counter {
        enum Type {
            VarInt
        };

    private:
        size_t pos;
        Type type;
        int32_t value;

    public:
        Counter(Type t, size_t pos);
        ~Counter();

        void up();
        void up(int32_t a) {value += a;}
        void shift(size_t pos);
        void insert(Packet &p);
        size_t len() const;
    };

private:
    std::vector<char> buff;
    char _id = 0x00;
    std::vector<Packet::Counter>* counter = nullptr;

public:
    std::vector<char>* _exposeBuffer() {return &buff;}

    void add(int a);
    void add(bool a);
    void add(char a);
    void add(short a);
    void add(uint64_t a);
    void add(float a);
    void add(double a);
    void add(std::string a);
    Packet::Counter* const add(Counter::Type a);

    void varInt(int32_t a);
    void string(const char* a);
    void string(std::string a);
    void byte(unsigned char a);

    void prefixedArray(std::vector<int> a);

    void id(char a) {_id = a;}

    void build();
    void send(SOCKET socket);
    void sendRaw(SOCKET socket);

    ~Packet() = default;
};


#endif //MCSERVER_PACKET_H