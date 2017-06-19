#include "SleepUtil.h"
#include <chrono>
#include <thread>

namespace Chip8 {
void SleepUtil::sleepMillis(int millis) { std::this_thread::sleep_for(std::chrono::milliseconds(millis)); }
}