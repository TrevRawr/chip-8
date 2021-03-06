#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H

#include <cstdint>
#include "../constants/OpcodeBitmasks.h"
#include "../constants/Opcodes.h"
#include "../exceptions/InstructionUnimplementedException.h"
#include "../storage/Memory.h"
#include "../subsystems/display/IDisplay.h"
#include "../subsystems/input/IInputController.h"

/**
 * The cpu is the heart of the emulator, and implements every opcode in the chip-8 specification
 * It executes opcodes, keeps track of and updates the chip-8 system state accordingly, and calls functionality where necessary of other
 * components
 * (ex: IDisplay) that are passed in as dependencies.
 */
namespace Chip8 {
class Cpu {
   public:
    static const int INDEX_CARRY_REGISTER = 15;
    static const uint16_t DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE = 2;
    static const int NUM_GENERAL_PURPOSE_REGISTERS = 16;

    Cpu(Memory &memory, IDisplay &display, IInputController &inputController);

    void emulateCycle();

    uint16_t getProgramCounter() const;

    uint8_t getRegisterValue(unsigned int registerNumber) const;

    uint16_t getIndexRegisterValue() const;

    uint8_t getDelayTimerValue() const;

    uint8_t getSoundTimerValue() const;

   private:
    // this includes the "carry-flag" register VF
    static const int NUM_STACK_LEVELS = 16;
    static const int NUM_OP_CODE_IMPLEMENTATIONS = 16;
    static const int NUM_ARITHMETIC_OPCODE_IMPLEMENTATIONS = 16;
    static const uint8_t BITMASK_REGISTER_FIRST_BIT = 0x80;
    static const uint8_t BITSHIFT_REGISTER_FIRST_TO_LAST = 7;

    uint8_t generalPurposeRegisters[NUM_GENERAL_PURPOSE_REGISTERS];
    uint16_t indexRegister;
    uint16_t programCounter;
    uint8_t delayTimerRegister;
    uint8_t soundTimerRegister;

    Memory &memory;
    IDisplay &display;
    IInputController &inputController;

    // this is not explicitly part of the chip-8 specification,
    // but will be required to keep track of the program counter
    // after jump instructions are used, so the program counter
    // can return to its previous location later
    uint16_t stack[NUM_STACK_LEVELS];
    int currStackLevel = 0;

    uint16_t fetchOpCode();

    void decodeAndExecuteOpcode(uint16_t opcode);

    void handleUnimplementedOpcode(uint16_t opcode);

    void delegateToArithmethicOpcodeImplementations(uint16_t opcode);

    void executeOpcodeBeginningWithZero(uint16_t opcode);

    // 0x00EE
    void executeReturnFromSubroutineOpcode();

    // 0x1NNN
    void executeJumpOpcode(uint16_t opcode);

    // 0x2XNN
    void executeCallSubroutineOpcode(uint16_t opcode);

    // 0x3XNN
    void executeRegisterEqualsValueOpcode(uint16_t opcode);

    void skipInstruction();

    // 0x4XNN
    void executeRegisterNotEqualsValueOpcode(uint16_t opcode);

    // 0x5XY0
    void executeRegisterEqualsRegisterOpcode(uint16_t opcode);

    // 0x6XNN
    void executeAssignRegisterOpcode(uint16_t opcode);

    // 0x7XNN
    void executeAddToRegisterOpcode(uint16_t opcode);

    // 0x8XY0
    void executeArithmeticSetOpcode(uint16_t opcode);

    // 0x8XY1
    void executeArithmeticSetOrOpcode(uint16_t opcode);

    // 0x8XY2
    void executeArithmeticSetAndOpcode(uint16_t opcode);

    // 0x8XY3
    void executeArithmeticSetXOROpcode(uint16_t opcode);

    // 0x8XY4
    void executeArithmeticAddOpcode(uint16_t opcode);

    // 0x8XY5
    void executeArithmeticSubtractOpcode(uint16_t opcode);

    // 0x8XY6
    void executeArithmeticShiftRightOpcode(uint16_t opcode);

    // 0x8XY7
    void executeArithmeticSubtractDifferenceOpcode(uint16_t opcode);

    // 0x8XYE
    void executeArithmeticShiftLeftOpcode(uint16_t opcode);

    // 0x9XY0
    void executeNotEqualsRegistersOpcode(uint16_t opcode);

    // 0xANNN
    void executeAssignOpcode(uint16_t opcode);

    // 0xBNNN
    void executeJumpToAddressPlusRegisterOpcode(uint16_t opcode);

    // 0xCNXX
    void executeRandomNumberOpcode(uint16_t opcode);

    // 0xDXYN
    void executeDrawSpriteOpcode(uint16_t opcode);

    // 0xEX9E and 0xEXA1
    void executeKeyPressedSkipOpcodes(uint16_t opcode);

    void delegateToFOpcodeImplementations(uint16_t opcode);

    // 0xFX07
    void executeSetRegisterToDelayTimerOpcode(uint16_t opcode);

    // 0xFX0A
    void executeBlockKeyPressesOpcode(uint16_t opcode);

    // 0xFX15
    void executeSetDelayTimerToRegisterOpcode(uint16_t opcode);

    // 0xFX18
    void executeSetSoundTimerToRegisterOpcode(uint16_t opcode);

    // 0xFX1E
    void executeAddRegisterToIndexRegisterOpcode(uint16_t opcode);

    // 0xFX29
    void executeSetSpriteLocationOpcode(uint16_t opcode);

    // 0xFX33
    void executeConvertToBCDOpcode(uint16_t opcode);

    // 0xFX55
    void executeRegisterDumpOpcode(uint16_t opcode);

    // 0xFX65
    void executeRegisterLoadOpcode(uint16_t opcode);

    typedef void (Cpu::*OpcodeMemberFunction)(uint16_t opcode);

    // an array of function pointers that point to functions that implement an opcode or opcodes where the first nibble
    // of the opcode is the index of the implementing function in the array
    OpcodeMemberFunction cpuOpcodeImplementations[NUM_OP_CODE_IMPLEMENTATIONS] = {&Cpu::executeOpcodeBeginningWithZero,
                                                                                  &Cpu::executeJumpOpcode,
                                                                                  &Cpu::executeCallSubroutineOpcode,
                                                                                  &Cpu::executeRegisterEqualsValueOpcode,
                                                                                  &Cpu::executeRegisterNotEqualsValueOpcode,
                                                                                  &Cpu::executeRegisterEqualsRegisterOpcode,
                                                                                  &Cpu::executeAssignRegisterOpcode,
                                                                                  &Cpu::executeAddToRegisterOpcode,
                                                                                  &Cpu::delegateToArithmethicOpcodeImplementations,
                                                                                  &Cpu::executeNotEqualsRegistersOpcode,
                                                                                  &Cpu::executeAssignOpcode,
                                                                                  &Cpu::executeJumpToAddressPlusRegisterOpcode,
                                                                                  &Cpu::executeRandomNumberOpcode,
                                                                                  &Cpu::executeDrawSpriteOpcode,
                                                                                  &Cpu::executeKeyPressedSkipOpcodes,
                                                                                  &Cpu::delegateToFOpcodeImplementations};

    OpcodeMemberFunction arithmeticOpcodeImplementations[NUM_ARITHMETIC_OPCODE_IMPLEMENTATIONS] = {
        &Cpu::executeArithmeticSetOpcode,        &Cpu::executeArithmeticSetOrOpcode,
        &Cpu::executeArithmeticSetAndOpcode,     &Cpu::executeArithmeticSetXOROpcode,
        &Cpu::executeArithmeticAddOpcode,        &Cpu::executeArithmeticSubtractOpcode,
        &Cpu::executeArithmeticShiftRightOpcode, &Cpu::executeArithmeticSubtractDifferenceOpcode,
        &Cpu::handleUnimplementedOpcode,         &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,         &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,         &Cpu::handleUnimplementedOpcode,
        &Cpu::executeArithmeticShiftLeftOpcode,  &Cpu::handleUnimplementedOpcode};

    int getFirstNibbleFromOpcode(uint16_t opcode) const;

    unsigned int getSecondNibbleFromOpcode(uint16_t opcode) const;

    int getThirdNibbleFromOpcode(uint16_t opcode) const;

    void setAdditionOverflowRegister(int registerNumberX, int registerNumberY);

    void setSubtractionXYOverflowRegisters(int registerNumberX, int registerNumberY);

    void setSubtractionYXOverflowRegisters(int registerNumberX, int registerNumberY);

    void setIndexOverflowRegister(int registerNumber);

    void updateTimers();
};
}

#endif  // CHIP_8_CPU_H
