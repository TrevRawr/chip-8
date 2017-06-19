#ifndef CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H
#define CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H


#include <exception>
#include "UnimplementedException.h"

/**
 * An exception thrown by opcode (instructions) implementations that are not yet (or won't be) implemented.
 */
namespace Chip8 {
class InstructionUnimplementedException : public UnimplementedException {
public:
    InstructionUnimplementedException(const std::string &message) : UnimplementedException(message) {}
};
}

#endif //CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H
