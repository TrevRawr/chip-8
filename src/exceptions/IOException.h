#ifndef CHIP_8_IOEXCEPTION_H
#define CHIP_8_IOEXCEPTION_H

#include "BaseException.h"

class IOException : public BaseException {
public:
    IOException(const std::string &message) : BaseException(message) {}
};
#endif //CHIP_8_IOEXCEPTION_H
