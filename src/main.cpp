#include <iostream>
#include "Chip8.h"
#include "subsystems/SdlSubsystemManager.h"

using namespace Chip8;

/**
 * A simple main function to kick off execution of the emulator.
 */
int main() {
    try {
        SdlSubsystemManager sdlSubsystemManager;
        Chip8Emulator chip8{sdlSubsystemManager};
        chip8.loadGameFile("c8games/PONG");
        chip8.beginEmulation();
    } catch (BaseException e) {
        std::cout << "Exception Encountered: " << e.what();
    }
    return 0;
}