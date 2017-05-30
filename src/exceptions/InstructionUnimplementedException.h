#ifndef CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H
#define CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H


#include <exception>
#include "UnimplementedException.h"

class InstructionUnimplementedException : public UnimplementedException {
public:
    InstructionUnimplementedException(const std::string &message) : UnimplementedException(message) {}
};


#endif //CHIP_8_METHODUNIMPLEMENTEDEXCEPTION_H
