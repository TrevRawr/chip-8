#include "SdlSubsystemManager.h"
#include "display/Display.h"
#include "input/InputController.h"

IInputController & SdlSubsystemManager::getInputController() {
    return *inputController;
}

IDisplay & SdlSubsystemManager::getDisplay() {
    return *display;
}

SdlSubsystemManager::SdlSubsystemManager() : display(new Display), inputController(new InputController){}

SdlSubsystemManager::~SdlSubsystemManager() {
    //manually destroy these smart pointers so they are freed before calling SDL_Quit()
    //I'm not sure if this is required by SDL (resource leaks seem to be the same either way),
    //but the documentation here seems to say it is: https://wiki.libsdl.org/SDL_Quit
    display.reset();
    inputController.reset();
    SDL_Quit();
}
