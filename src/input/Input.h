#ifndef CHIP_8_INPUT_H
#define CHIP_8_INPUT_H


class Input {
private:
    static const int NUM_KEYS = 16;
    bool keyPressedStates[NUM_KEYS];
};


#endif //CHIP_8_INPUT_H
