#ifndef CHIP_8_OPCODES_H
#define CHIP_8_OPCODES_H

#include <cstdint>

class Opcodes {
public:
    static const uint16_t CLEAR_DISPLAY = 0x00E0;
    static const uint16_t RETURN_FROM_SUBROUTINE = 0x00EE;
};

#endif //CHIP_8_OPCODES_H
