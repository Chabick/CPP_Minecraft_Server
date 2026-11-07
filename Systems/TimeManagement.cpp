#include "TimeManagement.h"

#include <chrono>

#include "Network/NetworkHandler.h"
#include "Util/Logger.h"

namespace System {
    void TimeManagement::startClock() {
        if (isRunning) return;
        isRunning = true;
        clockThread = std::thread(&TimeManagement::_clockFunction, this);
    }

    void TimeManagement::_clockFunction() {
        std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

        while (isRunning) {
            std::chrono::steady_clock::duration delta = t1-t0;
            if (delta.count() < 50.0) {
                std::chrono::duration<double, std::milli> delta_ms(200.0 - delta.count());
                std::this_thread::sleep_for(delta_ms);
            } else if (delta.count() < 100.0) {
                //warning for slower
            } else {
                //shutdown systems bc its running at half speed
            }

            t0 = std::chrono::steady_clock::now();

            actionKeepAlive();

            t1 = std::chrono::steady_clock::now();
        }
    }

    void TimeManagement::stopClock() {
        isRunning = false;
        clockThread.join();
    }

    bool TimeManagement::isClockRunning() const {
        return isRunning;
    }

    TimeManagement::__TimeStorage* TimeManagement::data = nullptr;

    TimeManagement::TimeManagement(NetworkHandler* network_handler) {
        if (data == nullptr) {
            data = new TimeManagement::__TimeStorage();
        }

        data->tm = this;

        networkHandler = network_handler;
    }

    TimeManagement::~TimeManagement() {
        if (isRunning) stopClock();

        delete data;
    }

    void TimeManagement::actionKeepAlive() {
        keepAliveCounter = (keepAliveCounter+1)%20;
        if (keepAliveCounter == 0) {
            networkHandler->distributeUpdate({Update::KEEPALIVE,{false}});
        }
    }
}
