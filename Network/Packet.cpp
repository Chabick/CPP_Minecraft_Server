#include "Packet.h"

#include "Types/TVarInt.h"
#include "Util/tools.h"

void Packet::add(bool a) {
    buff.insert(buff.end(), a);
}

void Packet::add(char a) {
    buff.insert(buff.end(), a);
}

void Packet::add(int a) {
    for (int i = 3; i >= 0; i--) {
        buff.insert(buff.end(), static_cast<char>(a>>i*8));
    }
}

void Packet::add(double a) {
    union {
        double d;
        char v[sizeof(double)];
    };
    for (int i = 7; i >= 0; i--) {
        buff.insert(buff.end(), v[i]);
    }
}

void Packet::add(std::string a) {}

void Packet::add(float a) {
    union {
        float d;
        char v[sizeof(float)];
    };
    for (int i = 3; i >= 0; i--) {
        buff.insert(buff.end(), v[i]);
    }
}

void Packet::add(long a) {
    for (int i = 7; i >= 0; i--) {
        buff.insert(buff.end(), static_cast<char>(a>>i*8));
    }
}

void Packet::add(short a) {}


#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80
void Packet::varInt(int32_t a) {
    while (true) {
        if ((a & ~SEGMENT_BITS) == 0) {
            buff.insert(buff.end(), a);
            return;
        }

        buff.insert(buff.end(), (a & SEGMENT_BITS) | CONTINUE_BIT);

        a >>= 7;
    }
}

void Packet::string(const char *a) {
    this->string(std::string(a));
}

void Packet::string(std::string a) {
    buff.insert(buff.end(), a.size());
    for (char c : a) {
        buff.insert(buff.end(), c);
    }
}

void Packet::byte(unsigned char a) {
    buff.insert(buff.end(), a);
}

void Packet::send(SOCKET socket) {
    buff.insert(buff.begin(), _id);

    auto send_buff = (char* )malloc(buff.size() + sizeVarInt(buff.size()));

    int size = buff.size();

    while (true) {
        if ((size & ~SEGMENT_BITS) == 0) {
            *send_buff = size;
            send_buff++;
            break;;
        }

        *send_buff = (size & SEGMENT_BITS) | CONTINUE_BIT;
        send_buff++;

        size >>= 7;
    }

    unsigned short offset = sizeVarInt(buff.size());

    for (int i = 0; i < buff.size(); i++) {
        send_buff[i+offset] = buff[i];
    }

    send_all(socket, send_buff, buff.size()+sizeVarInt(buff.size()));

    free(send_buff);
}
