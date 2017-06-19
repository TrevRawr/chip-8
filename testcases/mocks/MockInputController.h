#ifndef CHIP_8_MOCKINPUTCONTROLLER_H
#define CHIP_8_MOCKINPUTCONTROLLER_H

#include <cstdint>
#include <gmock/gmock-generated-function-mockers.h>
#include "../../src/subsystems/input/IInputController.h"

/**
 * A Google Mock implementation of IInputController for testing purposes.
 */
class MockInputController : public Chip8::IInputController {
public:
    MOCK_METHOD1(isKeyPressed, bool(unsigned int keyNumber));
    MOCK_METHOD0(checkForKeyPresses, void());
    MOCK_METHOD0(waitForKeyPress, uint8_t());
    MOCK_METHOD0(isExitButtonPressed, bool());

};
#endif //CHIP_8_MOCKINPUTCONTROLLER_H
