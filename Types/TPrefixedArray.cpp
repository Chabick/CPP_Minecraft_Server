#include "TPrefixedArray.h"

#include "Util/tools.h"
#include "Types/TVarInt.h"
#include <string>

template <typename T>
void writePrefixedArrayRaw(SOCKET socket, std::vector<T> items) {
    //TODO: implement this
}

template <>
void writePrefixedArrayRaw<int>(SOCKET socket, std::vector<int> items) {
    writeVarIntRaw(socket, items.size());
    for (int i : items) {
        writeVarIntRaw(socket, i);
    }
}