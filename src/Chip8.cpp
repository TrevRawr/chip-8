#include "Chip8.h"
#include "constants/Constants.h"
#include "exceptions/IOException.h"
#include "io/FileByteReader.h"
#include "utils/SleepUtil.h"

namespace Chip8 {
// needed for static class definition of this array to compile
constexpr unsigned char Chip8Emulator::DEFAULT_FONT_SET[FONTSET_BUFFER_SIZE];

void Chip8Emulator::beginEmulation() {
    // TODO: consider starting this in a separate thread so that the emulator can be stopped if desired
    isEmulating = true;
    while (isEmulating) {
        subsystemManager.getInputController().checkForKeyPresses();
        // TODO: change delay to actual processor clock rate
        SleepUtil::sleepMillis(1);
        cpu.emulateCycle();
        isEmulating = !subsystemManager.getInputController().isExitButtonPressed();
    }
}

void Chip8Emulator::stopEmulation() { isEmulating = false; }

void Chip8Emulator::loadFontToMemory() {
    for (unsigned int i = 0; i < FONTSET_BUFFER_SIZE; i++) {
        memory.setDataAtAddress(i + Constants::MEMORY_FONT_START_LOCATION, DEFAULT_FONT_SET[i]);
    }
}

void Chip8Emulator::loadGameFile(std::string game) {
    FileByteReader fileByteReader(game);
    const int bufferSize = Memory::NUM_BYTES_OF_MEMORY - Constants::MEMORY_PROGRAM_START_LOCATION;
    uint8_t buffer[bufferSize];
    long bytesRead = fileByteReader.readToBuffer(buffer, 0, bufferSize);
    if (bytesRead > 0) {
        for (unsigned int address = Constants::MEMORY_PROGRAM_START_LOCATION; address < Memory::NUM_BYTES_OF_MEMORY; address++) {
            memory.setDataAtAddress(address, buffer[address - Constants::MEMORY_PROGRAM_START_LOCATION]);
        }
    } else {
        throw IOException("Could not read any bytes in the file");
    }
}

Chip8Emulator::Chip8Emulator(ISubsystemManager &subsystemManager)
    : memory(Memory()),
      subsystemManager(subsystemManager),
      cpu(Cpu(memory, subsystemManager.getDisplay(), subsystemManager.getInputController())) {
    loadFontToMemory();
}
}