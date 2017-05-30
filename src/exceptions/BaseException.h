#ifndef CHIP_8_BASEEXCEPTION_H
#define CHIP_8_BASEEXCEPTION_H

#include <exception>
#include <string>

class BaseException : public std::exception {
public:
    BaseException(const std::string &message) : message(message) {};
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

#endif //CHIP_8_BASEEXCEPTION_H
