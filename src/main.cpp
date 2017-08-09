#include <iostream>
#include "Chip8.h"
#include "subsystems/SdlSubsystemManager.h"

using namespace Chip8;

/**
 * A simple main function to kick off execution of the emulator.
 */

// Expecting the program name as arg 1, and the ROM file name to load as arg 2
const int EXPECTED_NUM_ARGS = 2;
const int ROM_FILE_PATH_INDEX = 1;
int main(int argc, char **argv) {
    if (argc != EXPECTED_NUM_ARGS) {
        std::cout << "Incorrect usage. Expected Chip8 ROM file path as an argument" << std::endl;
        return 1;
    }
    try {
        SdlSubsystemManager sdlSubsystemManager;
        Chip8Emulator chip8{sdlSubsystemManager};
        chip8.loadGameFile(argv[ROM_FILE_PATH_INDEX]);
        chip8.beginEmulation();
    } catch (BaseException e) {
        std::cout << "Exception Encountered: " << e.what();
    }
    return 0;
}