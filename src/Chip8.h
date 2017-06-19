#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H


#include "cpu/Cpu.h"
#include "subsystems/ISubsystemManager.h"
#include <string>

/**
 * The "composer" of the chip-8 emulator that takes all the different components of the emulator and orchestrates them together.
 * This class provides a nice abstraction of the chip-8 system as a whole, and if this project was ever to be used as a library for another project,
 * this would likely be the API that would be exposed to external projects.
 * Currently, this class basically instantiates all necessary dependencies, but in the future, if there are multiple implementations of certain dependencies
 * (ex: IDisplay), then these may need to be injected through the constructor.
 */
class Chip8 {
public:
    Chip8(ISubsystemManager &subsystemManager);

    void loadGameFile(std::string game);
    void beginEmulation();
    void stopEmulation();
private:
    static const int FONTSET_BUFFER_SIZE = 80;
    static constexpr unsigned char DEFAULT_FONT_SET[FONTSET_BUFFER_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    bool isEmulating = false;
    Memory memory;
    ISubsystemManager& subsystemManager;
    Cpu cpu;

    void loadFontToMemory();
};


#endif //CHIP_8_CHIP8_H
