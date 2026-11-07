#ifndef MCSERVER_TIMEMANAGEMENT_H
#define MCSERVER_TIMEMANAGEMENT_H

#include <thread>
#include "Network/NetworkHandler.h"

namespace System {
    class TimeManagement {
    private:
        class __TimeStorage {
            friend class TimeManagement;
            TimeManagement *tm = nullptr;
        };

        static __TimeStorage* data;
        std::thread clockThread;

        void _clockFunction();

        bool isRunning = false;

        NetworkHandler *networkHandler;

        void actionKeepAlive();
        short keepAliveCounter = 0;

    public:
        TimeManagement(NetworkHandler* network_handler);
        ~TimeManagement();

        void startClock();
        void stopClock();
        bool isClockRunning() const;
    };
}


#endif //MCSERVER_TIMEMANAGEMENT_H