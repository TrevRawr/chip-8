#include "RandomUtil.h"

namespace Chip8 {
bool RandomUtil::isSeeded = false;

uint8_t RandomUtil::getRandomNumber() {
    if (!isSeeded) {
        srand((unsigned int)time(0));
    }
    return (uint8_t)(rand() % Constants::VALUES_IN_BYTE);
}
}