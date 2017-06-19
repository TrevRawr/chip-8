#include <chrono>
#include <thread>
#include "SleepUtil.h"

namespace Chip8 {
void SleepUtil::sleepMillis(int millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
}