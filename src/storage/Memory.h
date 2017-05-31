#ifndef CHIP_8_MEMORY_H
#define CHIP_8_MEMORY_H


#include <cstdint>

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
