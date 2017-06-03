#ifndef CHIP_8_CONSTANTS_H
#define CHIP_8_CONSTANTS_H
class Constants {
public:
    static const int BITS_IN_BYTE = 8;
    static const int VALUES_IN_BYTE = 256;
    static const int MEMORY_PROGRAM_START_LOCATION = 0x200;
    static const uint16_t MEMORY_FONT_START_LOCATION = 0x050;
    static const int MAX_BYTE_SIZE = 0xFF;
    static const int MAX_INDEX_REGISTER_VALUE = 0xFFF;
    static const uint8_t FONT_NUM_BYTES_PER_CHARACTER = 5;
};
#endif //CHIP_8_CONSTANTS_H
