#ifndef CHIP_8_INPUTCONTROLLER_H
#define CHIP_8_INPUTCONTROLLER_H


#include "IInputController.h"

/**
 * An implementation of IInputController using SDL. This is a fairly simple implementation
 * that doesn't do too much beyond mapping chip-8 input to a few keyboard keys.
 */
class InputController : public IInputController {
public:
    /**
     * @return whether or not the key at keyNumber is pressed. If keyNumber is larger than NUM_KEYS, returns false
     */
    bool isKeyPressed(unsigned int keyNumber) override;
    void checkForKeyPresses() override;
    /**
     * Blocks until the next keydown event occurs, and handles the next keydown event
     * @return the number of the key that was pressed
     */
    uint8_t waitForKeyPress() override;
private:
    static const int ERROR_NO_INPUT_HANDLED = -1;

    SDL_Keycode keyboardMappings[NUM_KEYS] = {
            SDLK_1, SDLK_2, SDLK_3, SDLK_4,
            SDLK_q, SDLK_w, SDLK_e, SDLK_r,
            SDLK_a, SDLK_s, SDLK_d, SDLK_f,
            SDLK_z, SDLK_x, SDLK_c, SDLK_v
    };
    bool keyPressedStates[NUM_KEYS];

    /**
     * @return the chip-8 key number mapped to the keyboard key if the specified pressedKey is mapped to a chip-8 key
     * ERROR_NO_INPUT_HANDLED otherwise. When ERROR_NO_INPUT_HANDLED is returned, no key is updated
     */
    int setKeyPressedState(SDL_Keycode pressedKey, bool state);

    /**
     * @return the chip-8 key number corresponding to the event that was handled (if it was handled)
     * Some input events (ex: pressing a keyboard key not mapped to any button, or unknown events) won't be handled.
     * If an event isn't handled, ERROR_NO_INPUT_HANDLED is returned.
     */
    int handleInputEvents(const SDL_Event &e);
};


#endif //CHIP_8_INPUTCONTROLLER_H
