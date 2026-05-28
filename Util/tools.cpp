#include "tools.h"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <stdexcept>

void reverser(char* string, int length)
{
    int i;
    for (i = 0; i < length / 2; i++)
    {
        string[length - 1 - i] ^= string[i];
        string[i] ^= string[length - 1 - i];
        string[length - 1 - i] ^= string[i];
    }
}

ssize_t send_all(SOCKET socket, void* buffer, ssize_t len) {
    const char* p = (const char *)buffer;
    ssize_t sent = 0;

    while (sent < len) {
        ssize_t n = send(socket, p+sent, len-sent, 0);
        if (n > 0) {
            sent += n;
            continue;
        }
        if (n == 0) {
            return -1;
        }

        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK || err == WSAEINTR) {
            //TODO: disconnect client
            return -1;
        }
        return -1;
    }

    return sent;
}

ssize_t debug_send_all(SOCKET socket, void* buffer, ssize_t len) {
    const char* p = (const char *)buffer;
    ssize_t sent = 0;

    while (sent < len) {
        printf(std::to_string(sent).c_str());
        ssize_t n = send(socket, p+sent, len-sent, 0);
        if (n > 0) {
            sent += n;
            continue;
        }
        if (n == 0) {
            return -1;
        }

        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK || err == WSAEINTR) {
            //TODO: disconnect client
            return -1;
        }
        return -1;
    }

    return sent;
}

ssize_t recv_all (int client_fd, void *buf, size_t n, uint8_t require_first) {
    char *p = (char *)buf;
    size_t total = 0;

    // If requested, exit early when first byte not immediately available
    if (require_first) {
        ssize_t r = recv(client_fd, p, 1, MSG_PEEK);
        if (r <= 0) {
            if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                return 0; // no first byte available yet
            }
            return -1; // error or connection closed
        }
    }

    // Busy-wait (with task yielding) until we get exactly n bytes
    while (total < n) {
        ssize_t r = recv(client_fd, p + total, n - total, 0);
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                //TODO: handle network timeout
            } else {
                return -1; // real error
            }
        } else if (r == 0) {
            // connection closed before full read
            return total;
        }
        total += r;
    }

    return total; // got exactly n bytes
}

ssize_t writeByte(SOCKET socket, unsigned char byte) {
    return send_all(socket, &byte, 1);
}

char readByte(SOCKET socket) {
    char byte;
    int iResult = recv(socket, &byte, 1, 0);
    if (iResult == 0) throw std::runtime_error("recv() failed");
    return byte;
}

void consume(SOCKET socket, size_t n) {
    char *buf = (char *)malloc(n);
    recv_all(socket, buf, n, false);
    free(buf);
}
