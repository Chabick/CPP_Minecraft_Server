#include "IDManagement.h"

namespace System {
    uint32_t IDManagement::gen() {
        pos32++;
        return pos32;
    }

    uint64_t IDManagement::gen64() {
        pos64++;
        return pos64;
    }

    uint32_t IDManagement::pos32 = 0;
    uint64_t IDManagement::pos64 = LIMIT_32_BIT;

}
