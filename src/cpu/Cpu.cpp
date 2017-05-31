#include "Cpu.h"
#include "../constants/Constants.h"
#include "../constants/OpcodeBitshifts.h"
#include "../utils/RandomUtil.h"

Cpu::Cpu(Memory &memory) : memory(memory) {
    programCounter = Constants::MEMORY_PROGRAM_START_LOCATION;
    indexRegister = 0;
    currStackLevel = 0;

    //TODO: figure out how to initialize these
//    generalPurposeRegisters = ???
//    delayTimerRegister = 0;
//    soundTimerRegister = 0;
}

void Cpu::emulateCycle() {
    uint16_t opcode = fetchOpCode();

    //TODO: make sure there are no opcodes that require a different amount of instructions fetched
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;

    decodeAndExecuteOpcode(opcode);
}

uint16_t Cpu::fetchOpCode() {
    //use bit shifting to concatenate the contents of two 8-bit memory addresses to combine them into one 16-bit op-code
    //note that one opcode is two program instructions from memory
    return memory.getDataAtAddress(programCounter) << Constants::BITS_IN_BYTE | memory.getDataAtAddress(programCounter + 1);
}

void Cpu::decodeAndExecuteOpcode(uint16_t opcode) {
    //here we use bit shifting to interpret the first 4 bits of the op-code as an integer index into the opcode implementations array
    //forgive the "this->*" syntax; it's an unfortunate necessity to implement this with an array of member function pointers
    (this->*cpuOpcodeImplementations[getFirstNibbleFromOpcode(opcode)])(opcode);
}

void Cpu::delegateToAritmethicOpcodeImplementations(uint16_t opcode) {
    //for all arithmetic opcodes (beginning with first nibble == 8), the last nibble determines the specific arithmetic operation
    return (this->*arithmeticOpcodeImplementations[opcode & OpcodeBitmasks::LAST_NIBBLE])(opcode);
}

void Cpu::handleUnimplementedOpcode(uint16_t opcode) {
    throw InstructionUnimplementedException("The specified opcode has not yet been implemented: " + opcode);
}

void Cpu::executeOpcodeBeginningWithZero(uint16_t opcode) {
    //there are multiple opcodes that start with zero, so we choose between them here
    //if there was only one opcode beginning with zero, we could have implemented a
    //function for that opcode with no switch statement similar to the below functions
    switch (opcode) {
        case Opcodes::CLEAR_DISPLAY:
            return;
        case Opcodes::RETURN_FROM_SUBROUTINE:
            //TODO: correctly implement this
            return;
        default:
            throw InstructionUnimplementedException("Opcode unimplemented. If you were trying to call the RCA 1802 program, this is intentionally unimplemented");
    }
}

void Cpu::executeAssignOpcode(uint16_t opcode) {
    indexRegister = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
}

void Cpu::executeJumpOpcode(uint16_t opcode) {
    programCounter = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
}

void Cpu::executeCallSubroutineOpcode(uint16_t opcode) {
    //Save the location of the programCounter before going to the specified subroutine, so we can return later.
    //if we didn't increment the program counter after fetching each instruction in emulateCycle(), we could just
    //set the stack to programCounter, but to "undo" this increment, we subtract programCounter by the amount added earlier
    stack[currStackLevel] = programCounter - DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
    currStackLevel++;
    return executeJumpOpcode(opcode);
}

void Cpu::executeRegisterEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] == value) {
        skipInstruction();
    }
}

void Cpu::skipInstruction() {
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
}

//TODO: refactor this to reuse code in executeRegisterEqualsValueOpcode()
void Cpu::executeRegisterNotEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] == value) {
        skipInstruction();
    }
}

void Cpu::executeValueEqualsValueOpcode(uint16_t opcode) {
    int value1 = getSecondNibbleFromOpcode(opcode);
    int value2 = getThirdNibbleFromOpcode(opcode);
    if (value1 == value2) {
        skipInstruction();
    }
}

void Cpu::executeAssignRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
}

void Cpu::executeAddToRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] += (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
}

void Cpu::executeArithmeticSetOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberY];
}

//TODO: refactor bitwise operation instructions to reduce code duplication
void Cpu::executeArithmeticSetOrOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] | generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticSetAndOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] & generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticSetXOROpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] ^ generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticAddOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getSecondNibbleFromOpcode(opcode);
    setAdditionOverflowRegister(registerNumberX, registerNumberY);
    //note that if this overflows, the overflowed result will start counting from zero again after the overflow occurs
    //this is c++'s default behaviour, so we don't have to do anything special to implement this
    generalPurposeRegisters[registerNumberX] += generalPurposeRegisters[registerNumberY];
}

//TODO: refactor SetSubtraction...() methods to reduce code duplication
void Cpu::setAdditionOverflowRegister(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberX] + generalPurposeRegisters[registerNumberY] > Constants::MAX_BYTE_SIZE) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeArithmeticSubtractOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getSecondNibbleFromOpcode(opcode);
    setSubtractionXYOverflowRegisters(registerNumberX, registerNumberY);
    generalPurposeRegisters[registerNumberX] -= generalPurposeRegisters[registerNumberY];
}

void Cpu::setSubtractionXYOverflowRegisters(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberX] - generalPurposeRegisters[registerNumberY] < 0) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeArithmeticShiftRightOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[INDEX_CARRY_REGISTER] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BIT);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] >> 1;
}

void Cpu::executeArithmeticSubtractDifferenceOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getSecondNibbleFromOpcode(opcode);
    setSubtractionYXOverflowRegisters(registerNumberX, registerNumberY);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberY] - generalPurposeRegisters[registerNumberX];
}

void Cpu::setSubtractionYXOverflowRegisters(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberY] - generalPurposeRegisters[registerNumberX] < 0) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeArithmeticShiftLeftOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[INDEX_CARRY_REGISTER] =
            (uint8_t) ((generalPurposeRegisters[registerNumberX] & OpcodeBitmasks::FIRST_BIT) >> OpcodeBitshifts::BIT_FIRST_TO_LAST);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] << 1;
}

int Cpu::getFirstNibbleFromOpcode(uint16_t opcode) const {
    int registerNumber = (opcode & OpcodeBitmasks::FIRST_NIBBLE) >> OpcodeBitshifts::NIBBLE_FIRST_TO_LAST;
    return registerNumber;
}

int Cpu::getSecondNibbleFromOpcode(uint16_t opcode) const {
    int registerNumber = (opcode & OpcodeBitmasks::SECOND_NIBBLE) >> OpcodeBitshifts::BYTE_FIRST_TO_LAST;
    return registerNumber;
}

int Cpu::getThirdNibbleFromOpcode(uint16_t opcode) const {
    int value2 = (opcode & OpcodeBitmasks::THIRD_NIBBLE) >> OpcodeBitshifts::NIBBLE;
    return value2;
}

void Cpu::executeNotEqualsRegistersOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    if (generalPurposeRegisters[registerNumberX] != generalPurposeRegisters[registerNumberY]) {
        skipInstruction();
    }
}

void Cpu::executeJumpToAddressPlusRegisterOpcode(uint16_t opcode) {
    programCounter = (opcode & OpcodeBitmasks::LAST_THREE_NIBBLES) + generalPurposeRegisters[0];
}

void Cpu::executeRandomNumberOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE) & RandomUtil::getRandomNumber();
}








