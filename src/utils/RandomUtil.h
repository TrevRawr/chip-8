#ifndef CHIP_8_RANDOMUTIL_H
#define CHIP_8_RANDOMUTIL_H


#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "../constants/Constants.h"

namespace Chip8 {
class RandomUtil {
public:
    /**
     * @return a random number between 0 and 255
     */
    static uint8_t getRandomNumber();

private:
    static bool isSeeded;
};
}

#endif //CHIP_8_RANDOMUTIL_H
