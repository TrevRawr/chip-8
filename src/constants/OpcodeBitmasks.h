#ifndef CHIP_8_BITMASKS_H
#define CHIP_8_BITMASKS_H

#include <cstdint>

class OpcodeBitmasks {
public:
    static const uint16_t MOST_SIGNFICANT_NIBBLE = 0xF000;
    static const uint16_t LEAST_SIGNIFICANT_NIBBLE = 0x000F;
    static const uint16_t LAST_THREE_NIBBLES = 0x0FFF;
};

#endif //CHIP_8_BITMASKS_H
