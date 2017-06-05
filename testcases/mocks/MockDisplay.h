#ifndef CHIP_8_MOCKDISPLAY_H
#define CHIP_8_MOCKDISPLAY_H


#include "../../src/display/IDisplay.h"
#include "gmock/gmock.h"

class MockDisplay : public IDisplay {
public:
    MOCK_METHOD3(setPixel, void(int x, int y, bool value));
    MOCK_METHOD2(getPixel, bool(int x, int y));
    MOCK_METHOD0(clearScreen, void());
    MOCK_METHOD0(updateScreen, void());
//    void setPixel(int x, int y, bool value) override;
//    bool getPixel(int x, int y) override;
//    void clearScreen() override;
//    void updateScreen() override;
};


#endif //CHIP_8_MOCKDISPLAY_H
