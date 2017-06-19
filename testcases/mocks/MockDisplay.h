#ifndef CHIP_8_MOCKDISPLAY_H
#define CHIP_8_MOCKDISPLAY_H

#include "../../src/subsystems/display/IDisplay.h"
#include "gmock/gmock.h"

/**
 * A Google Mock implementation of IDisplay for testing purposes.
 */
class MockDisplay : public Chip8::IDisplay {
   public:
    MOCK_METHOD3(setPixel, void(int x, int y, bool value));
    MOCK_METHOD2(getPixel, bool(int x, int y));
    MOCK_METHOD0(clearScreen, void());
    MOCK_METHOD0(updateScreen, void());
};

#endif  // CHIP_8_MOCKDISPLAY_H
