#ifndef CHIP_8_INITIALIZATIONEXCEPTION_H
#define CHIP_8_INITIALIZATIONEXCEPTION_H


#include "BaseException.h"

/**
 * An exception to be thrown for any class/module initialization errors that may occur.
 * For example, if a video module encounters an error loading on a particular system, this exception could be thrown
 */
class InitializationException : public BaseException {
public:
    InitializationException(const std::string &message) : BaseException(message) {}
};


#endif //CHIP_8_INITIALIZATIONEXCEPTION_H
