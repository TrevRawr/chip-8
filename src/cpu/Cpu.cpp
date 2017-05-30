#include "Cpu.h"
#include "../constants/Constants.h"

Cpu::Cpu(Memory &memory) : memory(memory) {
    programCounter = Constants::MEMORY_PROGRAM_START_LOCATION;
    indexRegister = 0;
    currStackLevel = 0;

    //TODO: figure out how to initialize these
//    generalPurposeRegisters = ???
//    delayTimerRegister = 0;
//    soundTimerRegister = 0;
}

CycleStatus Cpu::emulateCycle() {
    uint16_t opcode = fetchOpCode();

    //TODO: make sure there are no opcodes that require a different amount of instructions fetched
    int numInstructionsFetched = 2;
    programCounter += numInstructionsFetched;

    return decodeAndExecuteOpcode(opcode);
}

uint16_t Cpu::fetchOpCode() {
    //use bit shifting to concatenate the contents of two 8-bit memory addresses to combine them into one 16-bit op-code
    //note that one opcode is two program instructions from memory
    return memory.getDataAtAddress(programCounter) << Constants::BITS_IN_BYTE | memory.getDataAtAddress(programCounter + 1);
}

CycleStatus Cpu::decodeAndExecuteOpcode(uint16_t opcode) {
    //here we use bit shifting to interpret the first 4 bits of the op-code as an integer index into the opcode implementations array
    //forgive the "this->*" syntax; it's an unfortunate necessity to implement this with an array of member function pointers
    return (this->*cpuOpcodeImplementations[(opcode & OpcodeBitmasks::MOST_SIGNFICANT_NIBBLE) >> Constants::OPCODE_NIBBLE_BIG_TO_LITTLE])(opcode);
}

CycleStatus Cpu::delegateToAritmethicOpcodeImplementations(uint16_t opcode) {
    //TODO: find out why we use the least significant nibble here
    return (this->*arithmeticOpcodeImplementations[opcode & OpcodeBitmasks::LEAST_SIGNIFICANT_NIBBLE])(opcode);
}

CycleStatus Cpu::handleUnimplementedOpcode(uint16_t opcode) {
    throw InstructionUnimplementedException("The specified opcode has not yet been implemented: " + opcode);
}

CycleStatus Cpu::executeOpcodeBeginningWithZero(uint16_t opcode) {
    //there are multiple opcodes that start with zero, so we choose between them here
    //if there was only one opcode beginning with zero, we could have implemented a
    //function for that opcode with no switch statement similar to the below functions
    switch (opcode) {
        case Opcodes::CLEAR_DISPLAY:
            return INTERRUPT_CLEARSCREEN;
        case Opcodes::RETURN_FROM_SUBROUTINE:
            //TODO: correctly implement this
            return SUCCESS;
        default:
            throw InstructionUnimplementedException("Opcode unimplemented. If you were trying to call the RCA 1802 program, this is intentionally unimplemented");
    }
}

CycleStatus Cpu::executeAssignOpcode(uint16_t opcode) {
    indexRegister = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
    return SUCCESS;
}

CycleStatus Cpu::executeJumpOpcode(uint16_t opcode) {
    programCounter = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
    return SUCCESS;
}
