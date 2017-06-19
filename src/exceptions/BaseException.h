#ifndef CHIP_8_BASEEXCEPTION_H
#define CHIP_8_BASEEXCEPTION_H

#include <exception>
#include <string>

/**
 * The standard application that every other exception in the application derives from.
 * It supports passing in a string to the constructor as an error message to be displayed on failure.
 */
namespace Chip8 {
class BaseException : public std::exception {
   public:
    BaseException(const std::string &message) : message(message){};

    virtual ~BaseException() override {}

    const char *what() const noexcept override { return message.c_str(); }

   private:
    std::string message;
};
}

#endif  // CHIP_8_BASEEXCEPTION_H
