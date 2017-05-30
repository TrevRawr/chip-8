#include "Chip8.h"

int main() {
    Chip8 chip8;
    chip8.selectGameFile("pong");
    chip8.beginEmulation();

    return 0;
}