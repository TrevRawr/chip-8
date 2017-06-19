#include <chrono>
#include <thread>
#include "SleepUtil.h"

void SleepUtil::sleepMillis(int millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
