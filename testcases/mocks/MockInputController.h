#ifndef CHIP_8_MOCKINPUTCONTROLLER_H
#define CHIP_8_MOCKINPUTCONTROLLER_H

#include <cstdint>
#include <gmock/gmock-generated-function-mockers.h>
#include "../../src/input/IInputController.h"

class MockInputController : public IInputController {
public:
    MOCK_METHOD1(isKeyPressed, bool(unsigned int keyNumber));
    MOCK_METHOD0(checkForKeyPresses, void());
    MOCK_METHOD0(waitForKeyPress, uint8_t());

};
#endif //CHIP_8_MOCKINPUTCONTROLLER_H
