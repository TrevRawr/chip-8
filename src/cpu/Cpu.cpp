#include "Cpu.h"
#include "../constants/Constants.h"
#include "../constants/OpcodeBitshifts.h"

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
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;

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
    return (this->*cpuOpcodeImplementations[getFirstNibbleFromOpcode(opcode)])(opcode);
}

CycleStatus Cpu::delegateToAritmethicOpcodeImplementations(uint16_t opcode) {
    //for all arithmetic opcodes (beginning with first nibble == 8), the last nibble determines the specific arithmetic operation
    return (this->*arithmeticOpcodeImplementations[opcode & OpcodeBitmasks::LAST_NIBBLE])(opcode);
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

CycleStatus Cpu::executeCallSubroutineOpcode(uint16_t opcode) {
    //Save the location of the programCounter before going to the specified subroutine, so we can return later.
    //if we didn't increment the program counter after fetching each instruction in emulateCycle(), we could just
    //set the stack to programCounter, but to "undo" this increment, we subtract programCounter by the amount added earlier
    stack[currStackLevel] = programCounter - DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
    currStackLevel++;
    return executeJumpOpcode(opcode);
}

CycleStatus Cpu::executeRegisterEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] == value) {
        skipInstruction();
    }
    return SUCCESS;
}

void Cpu::skipInstruction() {
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
}

//TODO: refactor this to reuse code in executeRegisterEqualsValueOpcode()
CycleStatus Cpu::executeRegisterNotEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] == value) {
        skipInstruction();
    }
    return SUCCESS;
}

int Cpu::getSecondNibbleFromOpcode(uint16_t opcode) const {
    int registerNumber = (opcode & OpcodeBitmasks::SECOND_NIBBLE) >> OpcodeBitshifts::BYTE_FIRST_TO_LAST;
    return registerNumber;
}

CycleStatus Cpu::executeValueEqualsValueOpcode(uint16_t opcode) {
    int value1 = (opcode & OpcodeBitmasks::SECOND_NIBBLE) >> OpcodeBitshifts::BYTE_FIRST_TO_LAST;
    int value2 = (opcode & OpcodeBitmasks::THIRD_NIBBLE) >> OpcodeBitshifts::NIBBLE;
    if (value1 == value2) {
        skipInstruction();
    }
    return SUCCESS;
}

CycleStatus Cpu::executeAssignRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
    return SUCCESS;
}

CycleStatus Cpu::executeAddToRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] += (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
    return SUCCESS;
}

int Cpu::getFirstNibbleFromOpcode(uint16_t opcode) const {
    int registerNumber = (opcode & OpcodeBitmasks::FIRST_NIBBLE) >> OpcodeBitshifts::NIBBLE_FIRST_TO_LAST;
    return registerNumber;
}






