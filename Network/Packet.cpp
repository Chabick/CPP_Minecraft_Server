#include "Packet.h"

#include <bitset>

#include "Types/TVarInt.h"
#include "Util/tools.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Util/Logger.h"

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
    d = a;
    for (int i = 3; i >= 0; i--) {
        buff.insert(buff.end(), v[i]);
    }
}

void Packet::add(uint64_t a) {
    for (int i = 7; i >= 0; i--) {
        buff.insert(buff.end(), static_cast<char>((a>>i*8)&0xFF));
    }
}

void Packet::add(short a) {
    buff.insert(buff.end(), static_cast<char>((a>>8)&0xFF));
    buff.insert(buff.end(), static_cast<char>((a&0xFF)));
}

Packet::Counter* const Packet::add(Packet::Counter::Type a) {
    if (counter == nullptr) counter = new std::vector<Packet::Counter>();

    switch (a) {
        case Packet::Counter::VarInt:
            counter->push_back(Counter{Counter::VarInt, buff.size()});
            break;
    }
    return &(counter->at(counter->size()-1));
}

Packet::Counter::Counter(Type t, size_t pos) {
    this->type = t;
    this->pos = pos;
    this->value = 0;
}

void Packet::Counter::up() {
    ++value;
}

Packet::Counter::~Counter() {

}

size_t Packet::Counter::len() const {
    size_t size = 0;
    uint32_t test_v = value;

    switch (type) {
        case Packet::Counter::VarInt:
            while ((test_v & ~0x7F) != 0) {
                test_v >>= 7;
                size ++;
            }
            return size;
        default:
            return 0;
    }
}

void Packet::Counter::shift(size_t pos) {
    this->pos += pos;
}

void Packet::Counter::insert(Packet &p) {
    switch (type) {
        case Packet::Counter::VarInt:
            while (true) {
                if ((value & ~0x7F) == 0) {
                    p._exposeBuffer()->insert(p._exposeBuffer()->begin()+pos, value);
                    pos++;
                    return;
                }

                p._exposeBuffer()->insert(p._exposeBuffer()->begin()+pos, (value & 0x7F) | 0x80);
                pos++;
                value >>= 7;
            }
            break;
    }
}

void Packet::build() {
    size_t shift = 0;
    for (auto c : *counter) {
        c.shift(shift);
        shift += c.len();
        c.insert(*this);
    }
}

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
    this->varInt(a.size());
    for (char c : a) {
        buff.insert(buff.end(), c);
    }
}

void Packet::byte(unsigned char a) {
    buff.insert(buff.end(), a);
}

void Packet::prefixedArray(std::vector<int> a) {
    this->varInt(a.size());
    for (int i : a) {
        this->varInt(i);
    }
}

std::string vectorTostring(const std::vector<char>& vec)
{
    std::stringstream result;
    for (const auto& v : vec)
    {
        result
            << std::setfill('0') << std::setw(sizeof(v) * 2)
            << std::hex << +v;
    }
    return result.str();
}

void Packet::send(SOCKET socket) {
    /*if (this->counter != nullptr) { //now Handled by the build function
        size_t shift_size = 0;
        for (Counter &c : *this->counter) {
            c.shift(shift_size);
            shift_size += c.len();
            c.insert(*this);
        }
    }*/

    buff.insert(buff.begin(), _id);

    auto base = (char* )malloc(buff.size() + sizeVarInt(buff.size()));
    char* write_buff = base;
    char* send_buff = base;

    auto size = static_cast<int32_t>(buff.size());

    while (true) {
        if ((size & ~SEGMENT_BITS) == 0) {
            *write_buff = static_cast<char>(size);
            write_buff++;
            break;
        }

        *write_buff = (size & SEGMENT_BITS) | CONTINUE_BIT;
        write_buff++;

        size >>= 7;
    }

    /*std::bitset<8> x(buff.size()-sizeVarInt(buff.size()));
    std::cout << x << ":" << vectorTostring(buff) << std::endl;*/
    for (char i : buff) {
        *write_buff = i;
        write_buff++;
    }

    send_all(socket, send_buff, buff.size()+sizeVarInt(buff.size()));
    /*for (int i = 0; i < buff.size()+sizeVarInt(buff.size()); i++) {
        writeByte(socket, *send_buff);
        send_buff++;
    }*/

    free(base);
}

void Packet::sendRaw(SOCKET socket) {
    auto base = (char* )malloc(buff.size() + sizeVarInt(buff.size()));
    char* write_buff = base;
    char* send_buff = base;

    auto size = static_cast<int32_t>(buff.size());

    while (true) {
        if ((size & ~SEGMENT_BITS) == 0) {
            *write_buff = static_cast<char>(size);
            write_buff++;
            break;
        }

        *write_buff = (size & SEGMENT_BITS) | CONTINUE_BIT;
        write_buff++;

        size >>= 7;
    }

    /*std::bitset<8> x(buff.size()-sizeVarInt(buff.size()));
    std::cout << x << ":" << vectorTostring(buff) << std::endl;*/
    for (char i : buff) {
        *write_buff = i;
        write_buff++;
    }

    send_all(socket, send_buff, buff.size()+sizeVarInt(buff.size()));
    /*for (int i = 0; i < buff.size()+sizeVarInt(buff.size()); i++) {
        writeByte(socket, *send_buff);
        send_buff++;
    }*/

    free(base);
}
