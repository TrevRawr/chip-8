#ifndef CHIP_8_CONSTANTS_H
#define CHIP_8_CONSTANTS_H
class Constants {
public:
    static const int BITS_IN_BYTE = 8;
    static const int MEMORY_PROGRAM_START_LOCATION = 0x200;
    static const int MEMORY_FONT_START_LOCATION = 0x050;
    static const int MAX_BYTE_SIZE = 0xFF;
};
#endif //CHIP_8_CONSTANTS_H
