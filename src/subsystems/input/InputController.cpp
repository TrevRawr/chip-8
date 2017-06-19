#include "InputController.h"
#include <SDL.h>
#include <SDL_events.h>
#include "../../exceptions/InitializationException.h"

namespace Chip8 {
bool InputController::isKeyPressed(unsigned int keyNumber) {
    if (keyNumber >= NUM_KEYS) {
        return false;
    }
    return keyPressedStates[keyNumber];
}

void InputController::checkForKeyPresses() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        handleInputEvents(e);
    }
}

uint8_t InputController::waitForKeyPress() {
    // note that this error value should never be returned
    // because the function will block until a value other than the error value is returned
    int keyPressed = ERROR_NO_INPUT_HANDLED;
    bool isKeyPressed = false;
    while (!isKeyPressed) {
        SDL_Event e;
        SDL_PollEvent(&e);
        // it's not enough for a key to be pressed, it has to be a key that's actually mapped to a chip-8 button
        // If a KEYDOWN event is handled, then it is mapped to a chip-8 button
        if (e.type == SDL_KEYDOWN) {
            keyPressed = handleInputEvents(e);
            if (keyPressed != ERROR_NO_INPUT_HANDLED) {
                isKeyPressed = true;
            }
        } else {
            // handle events of type other than KEYDOWN (like KEYUP) to ensure the keyPressedStates[] get updated correctly
            handleInputEvents(e);
        }
    }
    return (uint8_t)keyPressed;
}

int InputController::handleInputEvents(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode pressedKey = e.key.keysym.sym;
        return setKeyPressedState(pressedKey, true);
    } else if (e.type == SDL_KEYUP) {
        SDL_Keycode releasedKey = e.key.keysym.sym;
        return setKeyPressedState(releasedKey, false);
    } else if (e.type == SDL_QUIT) {
        isExitPressed = true;
    }
    return ERROR_NO_INPUT_HANDLED;
}

int InputController::setKeyPressedState(SDL_Keycode pressedKey, bool state) {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keyboardMappings[i] == pressedKey) {
            keyPressedStates[i] = state;
            return i;
        }
    }
    return ERROR_NO_INPUT_HANDLED;
}

bool InputController::isExitButtonPressed() { return isExitPressed; }

InputController::InputController() {
    if (SDL_Init(SDL_INIT_EVENTS) < 0) {
        throw InitializationException("Input Events subsystem could not be initialized");
    }
}

InputController::~InputController() { SDL_QuitSubSystem(SDL_INIT_EVENTS); }
}