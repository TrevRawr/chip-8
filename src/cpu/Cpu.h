#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H


#include <cstdint>
#include "CycleStatus.h"
#include "../storage/Memory.h"
#include "../constants/OpcodeBitmasks.h"
#include "../constants/Opcodes.h"
#include "../exceptions/InstructionUnimplementedException.h"

class Cpu {
public:
    Cpu(Memory &memory);

    CycleStatus emulateCycle();

private:
    static const int NUM_GENERAL_PURPOSE_REGISTERS = 15;
    static const uint16_t MEMORY_MAP_INTERPRETER_END = 0x1FF;
    static const int NUM_STACK_LEVELS = 16;
    static const int NUM_OP_CODE_IMPLEMENTATIONS = 16;
    static const int NUM_ARITHMETIC_OPCODE_IMPLEMENTATIONS = 17;

    uint8_t generalPurposeRegisters[NUM_GENERAL_PURPOSE_REGISTERS];
    uint16_t indexRegister;
    uint16_t programCounter;
    uint8_t delayTimerRegister;
    uint8_t soundTimerRegister;

    Memory& memory;

    //this is not explicitly part of the chip-8 specification,
    //but will be required to keep track of the program counter
    //after jump instructions are used, so the program counter
    //can return to its previous location later
    uint16_t stack[NUM_STACK_LEVELS];
    int currStackLevel;

    uint16_t fetchOpCode();

    CycleStatus decodeAndExecuteOpcode(uint16_t opcode);

    CycleStatus handleUnimplementedOpcode(uint16_t opcode);

    CycleStatus delegateToAritmethicOpcodeImplementations(uint16_t opcode);

    CycleStatus executeOpcodeBeginningWithZero(uint16_t opcode);

    CycleStatus executeAssignOpcode(uint16_t opcode);

    CycleStatus executeJumpOpcode(uint16_t opcode);

    //an array of function pointers that point to functions that implement an opcode or opcodes where the first nibble
    //of the opcode is the index of the implementing function in the array
    typedef CycleStatus (Cpu::*MemberFunction) (uint16_t opcode);
    MemberFunction cpuOpcodeImplementations[NUM_OP_CODE_IMPLEMENTATIONS] = {
        &Cpu::executeOpcodeBeginningWithZero, &Cpu::executeJumpOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::delegateToAritmethicOpcodeImplementations, &Cpu::handleUnimplementedOpcode, &Cpu::executeAssignOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode
    };

    MemberFunction arithmeticOpcodeImplementations[NUM_ARITHMETIC_OPCODE_IMPLEMENTATIONS] = {
            &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode, &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode,
        &Cpu::handleUnimplementedOpcode
    };

};

#endif //CHIP_8_CPU_H
