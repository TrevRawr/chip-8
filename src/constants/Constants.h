#ifndef CHIP_8_CONSTANTS_H
#define CHIP_8_CONSTANTS_H
class Constants {
public:
    static const int BITS_IN_BYTE = 8;
    //a nibble is half a byte (4 bits). Since opcodes are 16 bits, shifting a nibble by 12 will position it
    //from the first (big, most significant) 4 bits to last (little, least significant) 4 bits
    static const int OPCODE_NIBBLE_BIG_TO_LITTLE = 12;
    static const int MEMORY_PROGRAM_START_LOCATION = 0x200;
};
#endif //CHIP_8_CONSTANTS_H
