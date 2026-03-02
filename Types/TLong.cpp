#include "TLong.h"

#include <cstdint>

#include "Util/tools.h"

void writeLongRaw(SOCKET socket, int64_t data) {
    send_all(socket, &data, 8);
}

bool checkLittleEndian() {
    short int i = 0x1;
    char *c = (char *)&i;
    return c[0]==1; //little endian
}

int64_t readLongRaw(SOCKET socket) {
    unsigned char buffer[8];
    for (int i=0; i<8; i++) {buffer[i]=0x00;}

    for (int i = 0; i < 8; i++) {
        buffer[i] = (unsigned char)readByte(socket);
        printf("%x ", buffer[i]);
    }
    printf("\n");

    return
        ((uint64_t)buffer[0] << 56) |
        ((uint64_t)buffer[1] << 48) |
        ((uint64_t)buffer[2] << 40) |
        ((uint64_t)buffer[3] << 32) |
        ((uint64_t)buffer[4] << 24) |
        ((uint64_t)buffer[5] << 16) |
        ((uint64_t)buffer[6] << 8) |
        ((uint64_t)buffer[7]);

    /*
    //reverse from big to little endian to be able to shift the last the most
    reverser((char *)buffer, 8);

    //shift from most significant to most significant place
    for (int i = 7; i >= 0; i--) {
        data = data | (((int64_t)buffer[i]) << 8*i);
    }

    printf("%lld\n", data);

    return data;*/
}

char* readLong(SOCKET socket) {
    char* buffer = (char *) malloc(8);
    for (int i=0; i<8; i++) {buffer[i]=0x00;}

    recv_all(socket, buffer, 8, false);
    /*for (int i = 0; i < 8; i++) {
        buffer[i] = (unsigned char)readByte(socket);
        printf("%x ", buffer[i]);
    }
    printf("\n");
    */

    return buffer;
}