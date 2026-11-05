#include "TDouble.h"
#include "Util/tools.h"

#include <cstdint>

double readDoubleRaw(SOCKET socket) {
    uint64_t value = 0;

    for (int i = 0; i < 8; i++) {
        char a = readByte(socket);
        value <<= 8;
        value = value | a;
    }

    return std::bit_cast<double>(value);
}