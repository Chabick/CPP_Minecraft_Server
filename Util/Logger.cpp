#include "Logger.h"

int __debugger_linecount__ = 0;

#ifdef LOGGER

void _logger(const char* symbol, const char* buff) {
    std::cout
        << "[" << symbol << "] "
        << buff
        << std::endl;
}

void _linelogger() {
    std::cout
        << "[POS] " << __debugger_linecount__ << std::endl;
    __debugger_linecount__++;
}

#endif
