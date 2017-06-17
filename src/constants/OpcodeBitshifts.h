#ifndef CHIP_8_OPCODEBITSHIFTS_H
#define CHIP_8_OPCODEBITSHIFTS_H

/**
 * Bit shift amounts used to help access data stored in opcodes.
 */
class OpcodeBitshifts {
public:
    //a nibble is half a byte (4 bits). Since opcodes are 16 bits, shifting a nibble by 12 will position it
    //from the first (big, most significant) 4 bits to last (little, least significant) 4 bits
    static const int NIBBLE_THREE = 12;
    static const int NIBBLE_TWO = 8;
    static const int NIBBLE = 4;
    static const int BIT_FIRST_TO_LAST = 15;
};

#endif //CHIP_8_OPCODEBITSHIFTS_H
