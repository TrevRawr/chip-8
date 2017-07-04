#ifndef CHIP_8_BITMASKS_H
#define CHIP_8_BITMASKS_H

#include <cstdint>

/**
 * Bitmasks used to extract data from opcodes.
 */
namespace Chip8 {
class OpcodeBitmasks {
   public:
    // Note: a nibble is 4 bits (half a byte).
    // This is used frequently in chip-8 opcodes,
    // as the opcodes typically have "arguments" that are a nibble long
    static const uint16_t FIRST_NIBBLE = 0xF000;
    static const uint16_t SECOND_NIBBLE = 0x0F00;
    static const uint16_t THIRD_NIBBLE = 0x00F0;
    static const uint16_t LAST_NIBBLE = 0x000F;
    static const uint16_t LAST_THREE_NIBBLES = 0x0FFF;
    static const uint16_t LAST_BYTE = 0x00FF;
    static const uint16_t FIRST_BYTE = 0xFF00;
    static const uint16_t LAST_BIT = 0x0001;
    static const uint16_t FIRST_BIT = 0x8000;
};
}
#endif  // CHIP_8_BITMASKS_H
