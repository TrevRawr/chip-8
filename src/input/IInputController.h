#ifndef CHIP_8_IINPUTCONTROLLER_H
#define CHIP_8_IINPUTCONTROLLER_H


class IInputController {
public:
    static const int NUM_KEYS = 16;

    virtual ~IInputController() {};
    /**
     * @return whether or not the key at keyNumber is pressed. If keyNumber is larger than NUM_KEYS, returns false
     */
    virtual bool isKeyPressed(unsigned int keyNumber)= 0;

    virtual void checkForKeyPresses()= 0;

    /**
     * Blocks until the next keydown event occurs, and handles the next keydown event
     * @return the number of the key that was pressed
     */
    virtual uint8_t waitForKeyPress()= 0;
};


#endif //CHIP_8_IINPUTCONTROLLER_H
