#include "TString.h"

#include "TVarInt.h"
#include "Util/tools.h"

ReadBufferString readString(char* buffer, int bufferSize, int StartingPos) {
    ReadBufferInt length = readVarInt(buffer, bufferSize, StartingPos);

    //printf("String length: %d\n", length.result);

    std::string str(length.result, 0x5F);
    for (int i = 0; i < length.result; i++) {
        str[i] = buffer[length.pointer + i];
    }

    return ReadBufferString{buffer, bufferSize, length.pointer+length.result, str, length.result};
}

ReadBufferString readString(BufferReader reader) {
    ReadBufferInt length = readVarInt(reader.buffer, reader.size, reader.pointer);

    //printf("String length: %d\n", length.result);

    std::string str(length.result, 0x5F);
    for (int i = 0; i < length.result; i++) {
        str[i] = reader.buffer[length.pointer + i];
    }

    return ReadBufferString{reader.buffer, reader.size, length.pointer+length.result, str, length.result};
}

std::string readStringRaw(SOCKET socket) {
    int length = readVarIntRaw(socket);

    std::string str(length, 0x5F); //underscores
    for (int i = 0; i < length; i++) {
        str[i] = readByte(socket);
    }

    return std::string{str};
}

void writeStringRaw(SOCKET socket, std::string str) {
    int length = str.size();
    writeVarIntRaw(socket, length);
    for (int i = 0; i < length; i++) {
        writeByte(socket, str[i]);
    }
}

int sizeStringRaw(std::string str) {
    return sizeVarInt(str.size()) + str.size();
}