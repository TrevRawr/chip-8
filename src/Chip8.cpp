#include "Chip8.h"
#include "constants/Constants.h"

void Chip8::beginEmulation() {
    //TODO: consider starting this in a separate thread so that the emulator can be stopped if desired
    isEmulating = true;
    while (isEmulating) {
        CycleStatus status = cpu.emulateCycle();
        if (status == CycleStatus::INTERRUPT_DRAWSCREEN) {
            //TODO: have display module update screen
        }
    }
}

void Chip8::stopEmulation() {
    isEmulating = false;
}

//needed for static class definition of this array to compile
constexpr unsigned char Chip8::DEFAULT_FONT_SET[FONTSET_BUFFER_SIZE];

Chip8::Chip8() : memory(Memory()), cpu(Cpu(memory)) {
    loadFontToMemory();
}

void Chip8::loadFontToMemory() {
    for (unsigned int i = 0; i < FONTSET_BUFFER_SIZE; i++) {
        memory.setDataAtAddress(i + Constants::MEMORY_FONT_START_LOCATION, DEFAULT_FONT_SET[i]);
    }
}

void Chip8::selectGameFile(std::string game) {
    throw UnimplementedException("Selecting game file for file " + game + " not yet implemented");
}
