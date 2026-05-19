#ifndef MCSERVER_TPREFIXEDARRAY_H
#define MCSERVER_TPREFIXEDARRAY_H

#include <winsock2.h>
#include <vector>

template <typename T>
void writePrefixedArrayRaw(SOCKET socket, std::vector<T> items);

template <>
void writePrefixedArrayRaw<int>(SOCKET socket, std::vector<int> items);


#endif //MCSERVER_TPREFIXEDARRAY_H