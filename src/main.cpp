#include "Chip8.h"

/**
 * A simple main function to kick off execution of the emulator.
 */
int main() {
    Chip8 chip8;
    chip8.loadGameFile("c8games/PONG");
    chip8.beginEmulation();

    return 0;
}