#ifndef CHIP_8_OPCODEBITSHIFTS_H
#define CHIP_8_OPCODEBITSHIFTS_H

class OpcodeBitshifts {
public:
    //a nibble is half a byte (4 bits). Since opcodes are 16 bits, shifting a nibble by 12 will position it
    //from the first (big, most significant) 4 bits to last (little, least significant) 4 bits
    static const int NIBBLE_FIRST_TO_LAST = 12;
    static const int BYTE_FIRST_TO_LAST = 8;
    static const int NIBBLE = 4;
};

#endif //CHIP_8_OPCODEBITSHIFTS_H
