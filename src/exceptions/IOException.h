#ifndef CHIP_8_IOEXCEPTION_H
#define CHIP_8_IOEXCEPTION_H

#include "BaseException.h"

/**
 * An exception thrown when anything goes wrong in an input/output scenario,
 * For example, if there's an error reading a file, this exception may be thrown.
 */
namespace Chip8 {
class IOException : public BaseException {
   public:
    IOException(const std::string &message) : BaseException(message) {}
};
}

#endif  // CHIP_8_IOEXCEPTION_H
