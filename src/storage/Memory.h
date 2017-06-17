#ifndef CHIP_8_MEMORY_H
#define CHIP_8_MEMORY_H


#include <cstdint>

/**
 * A class that emulates the chip-8's memory. The memory consists of 4096 8-bit (1 byte) blocks of memory.
 * This class allows controlled access to memory, and provides bounds checking to ensure no illegal memory access occurs.
 */
class Memory {
public:
    static const int NUM_BYTES_OF_MEMORY = 4096;

    uint8_t getDataAtAddress(unsigned int address);
    void setDataAtAddress(unsigned int address, uint8_t data);
private:
    uint8_t memory[NUM_BYTES_OF_MEMORY];

    void checkAddressInBounds(unsigned int address);
};


#endif //CHIP_8_MEMORY_H
