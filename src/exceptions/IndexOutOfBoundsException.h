#ifndef CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H
#define CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H

#include "BaseException.h"

/**
 * An exception to be thrown when any access is made to data that is outside the data's boundaries.
 * For example, a list might use it if a user tries to access a list item that has an index number larger than the size of the list.
 */
namespace Chip8 {
class IndexOutOfBoundsException : public BaseException {
   public:
    IndexOutOfBoundsException(const std::string &message) : BaseException(message) {}
};
}

#endif  // CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H
