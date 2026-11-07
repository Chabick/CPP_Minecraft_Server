#ifndef MCSERVER_LOGGER_H
#define MCSERVER_LOGGER_H



#define LOGGER
#ifdef LOGGER
//logger is active

#include <iostream>
void _logger(const char* symbol, const char* buff);
extern int __debugger_linecount__;
void _linelogger();

#define LOGGER_LEVEL_USR //user
#define LOGGER_LEVEL_INF //info
#define LOGGER_LEVEL_WARN //warning
#define LOGGER_LEVEL_ERR //error
#define LOGGER_LEVEL_DEBUG //debug
#define LOGGER_LEVEL_LINECONTROL //for checking to which line it came

#ifdef LOGGER_LEVEL_USR
#else
#endif

#ifdef LOGGER_LEVEL_INF
#define info(x) _logger("I", x)
#else
#define info(x)
#endif

#ifdef LOGGER_LEVEL_WARN
#else
#endif

#ifdef LOGGER_LEVEL_ERR
#else
#endif

//debug logger
#ifdef LOGGER_LEVEL_DEBUG

#define lgdb(x) _logger("D", x)

#else

#define lgdb(x)

#endif

#ifdef LOGGER_LEVEL_LINECONTROL
#define l _linelogger()
#define ll _logger("LL", "--+");l;_logger("LL", "--")
#else
#define l
#define ll
#endif



#else
#define info(x)
#define lgdb(x)
#define l
#define ll
#endif



#endif //MCSERVER_LOGGER_H