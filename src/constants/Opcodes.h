#ifndef CHIP_8_OPCODES_H
#define CHIP_8_OPCODES_H

#include <cstdint>

/**
 * Constants that identify opcodes. Note that these constants don't contain the entire opcode, just enough information to uniquely identify
 * them
 * There is also not an entry for every opcode. Entries here are added as they are needed to implement things elsewhere.
 */
namespace Chip8 {
class Opcodes {
   public:
    static const uint16_t CLEAR_DISPLAY = 0x00E0;
    static const uint16_t RETURN_FROM_SUBROUTINE = 0x00EE;
    static const uint16_t KEYPRESS_SKIP_IF_PRESSED = 0x9E;
    static const uint16_t KEYPRESS_SKIP_IF_NOT_PRESSED = 0xA1;

    // all opcodes beginning with F
    // only the last byte of the opcode (which uniquely identifies the opcode) is shown
    static const int SET_REGISTER_TO_DELAY_TIMER = 0x07;
    static const int BLOCK_KEY_PRESSES = 0x0A;
    static const int SET_DELAY_TIMER_TO_REGISTER = 0x15;
    static const int SET_SOUND_TIMER_TO_REGISTER = 0x18;
    static const int ADD_REGISTER_TO_INDEX_REGISTER = 0x1E;
    static const int SET_SPRITE_LOCATION = 0x29;
    static const int CONVERT_TO_BCD = 0x33;
    static const int REGISTER_DUMP = 0x55;
    static const int REGISTER_LOAD = 0x65;
};
}

#endif  // CHIP_8_OPCODES_H
