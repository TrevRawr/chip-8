#include "Memory.h"
#include "../exceptions/IndexOutOfBoundsException.h"

namespace Chip8 {
uint8_t Memory::getDataAtAddress(unsigned int address) {
    checkAddressInBounds(address);
    return memory[address];
}

void Memory::setDataAtAddress(unsigned int address, uint8_t data) {
    checkAddressInBounds(address);
    memory[address] = data;
}

void Memory::checkAddressInBounds(unsigned int address) {
    if (address >= NUM_BYTES_OF_MEMORY) {
        throw IndexOutOfBoundsException("Address can't be bigger than memory size: " + NUM_BYTES_OF_MEMORY);
    }
}
}