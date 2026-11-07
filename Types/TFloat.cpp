#include "TFloat.h"

#include <cstdint>

#include "Util/tools.h"

float readFloatRaw(SOCKET socket) {
    uint32_t value = 0;

    for (int i = 0; i < 4; i++) {
        char a = readByte(socket);
        value <<= 8;
        value |= a;
    }

    return std::bit_cast<float>(value);
}
