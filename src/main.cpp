#include "Chip8.h"

int main() {
    Chip8 chip8;
    chip8.loadGameFile("c8games/PONG");
    chip8.beginEmulation();

    return 0;
}