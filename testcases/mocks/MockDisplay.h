#ifndef CHIP_8_MOCKDISPLAY_H
#define CHIP_8_MOCKDISPLAY_H


#include "../../src/display/IDisplay.h"
#include "gmock/gmock.h"

class MockDisplay : public IDisplay {
public:
    MOCK_METHOD3(setPixel, void(int x, int y, bool value));
    MOCK_METHOD2(getPixel, bool(int x, int y));
    MOCK_METHOD0(clearScreen, void());

    void updateScreen() override {
        //do nothing; this is called every time by cpu.executeCycle(), and we don't care about testing for it in every test case
    }
};


#endif //CHIP_8_MOCKDISPLAY_H
