#ifndef MCSERVER_IDMANAGEMENT_H
#define MCSERVER_IDMANAGEMENT_H
#include <cstdint>
#include <vector>

constexpr uint32_t LIMIT_32_BIT = 4294967295; //32 bit limit
using Id = uint32_t;
using Id64 = uint64_t;

namespace System {
    class IDManagement {
    private:
        static uint32_t pos32;
        static uint64_t pos64;

    public:
        static uint32_t gen();
        static uint64_t gen64();
    };
}


#endif //MCSERVER_IDMANAGEMENT_H