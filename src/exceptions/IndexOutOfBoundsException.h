#ifndef CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H
#define CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H

#include "BaseException.h"

class IndexOutOfBoundsException : public BaseException {
public:
    IndexOutOfBoundsException(const std::string &message) : BaseException(message) {}
};


#endif //CHIP_8_INDEXOUTOFBOUNDSEXCEPTION_H
