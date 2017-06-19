#ifndef CHIP_8_UNIMPLEMENTEDEXCEPTION_H
#define CHIP_8_UNIMPLEMENTEDEXCEPTION_H

#include "BaseException.h"

/**
 * An exception to be thrown by any function/method that is unimplemented.
 */
namespace Chip8 {
class UnimplementedException : public BaseException {
   public:
    UnimplementedException(const std::string &message) : BaseException(message) {}
};
}

#endif  // CHIP_8_UNIMPLEMENTEDEXCEPTION_H
