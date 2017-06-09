#include "gtest/gtest.h"
#include "CpuTestFixture.h"
#include "../src/constants/Constants.h"
#include "../src/constants/OpcodeBitshifts.h"
#include "../src/exceptions/IndexOutOfBoundsException.h"

void setOpcode(Memory& memory, uint16_t address, uint16_t opcode) {
    memory.setDataAtAddress(address, (uint8_t) ((opcode & OpcodeBitmasks::FIRST_BYTE) >> OpcodeBitshifts::NIBBLE_TWO));
    memory.setDataAtAddress(address + 1, (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE));
}

//0x00E0
TEST_F(CpuTestFixture, ClearScreen) {
    uint16_t clearScreenOpcode = 0x00E0;
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, clearScreenOpcode);

    EXPECT_CALL(display, clearScreen());
    cpu.emulateCycle();
}

//0x00EE and 0x2NNN
TEST_F(CpuTestFixture, Subroutine) {
    //test jumping to a subroutine. Address was chosen randomly and is not significant in any way
    uint16_t addressOfSubroutine = 0x300;
    uint16_t jumpToSubroutineOpcode = (uint16_t) ((2 << OpcodeBitshifts::NIBBLE_THREE) | addressOfSubroutine);
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, jumpToSubroutineOpcode);
    cpu.emulateCycle();
    EXPECT_EQ(cpu.getProgramCounter(), addressOfSubroutine);

    //test returning from subroutine
    uint16_t returnFromSubroutineOpcode = 0x00EE;
    setOpcode(memory, addressOfSubroutine, returnFromSubroutineOpcode);
    cpu.emulateCycle();
    EXPECT_EQ(cpu.getProgramCounter(), Constants::MEMORY_PROGRAM_START_LOCATION + Cpu::DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE);
}

TEST_F(CpuTestFixture, InvalidSubroutineReturn) {
    uint16_t returnFromSubroutineOpcode = 0x00EE;
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, returnFromSubroutineOpcode);
    EXPECT_THROW(cpu.emulateCycle(), IndexOutOfBoundsException);
}

static const int MAX_STACK_SIZE = 16;

TEST_F(CpuTestFixture, SubroutineStackOverflow) {
    //setup an infinite loop to call the same subroutine over and over again
    uint16_t addressOfSubroutine = 0x300;
    uint16_t jumpToSubroutineOpcode = (uint16_t) ((2 << OpcodeBitshifts::NIBBLE_THREE) | addressOfSubroutine);
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, jumpToSubroutineOpcode);
    setOpcode(memory, addressOfSubroutine, jumpToSubroutineOpcode);

    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        cpu.emulateCycle();
    }
    EXPECT_THROW(cpu.emulateCycle(), IndexOutOfBoundsException);
}

//0x1NNN
TEST_F (CpuTestFixture, JumpOpcode) {
    //expected address is random and insignificant
    uint16_t expectedAddress = 0x205;
    uint16_t jumpToAddressOpcode = (uint16_t) (0x1000 | expectedAddress);
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, jumpToAddressOpcode);
    cpu.emulateCycle();
    EXPECT_EQ(cpu.getProgramCounter(), expectedAddress);
};

void setRegister(Memory& memory, Cpu& cpu, int registerNumber, uint8_t registerValue) {
    uint16_t setRegisterOpcode = (uint16_t)
            ((6 << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
            registerValue);
    setOpcode(memory, cpu.getProgramCounter(), setRegisterOpcode);
    cpu.emulateCycle();
}

void expectInstructionSkipped(Cpu& cpu) {
    int previousProgramCounter = cpu.getProgramCounter();
    cpu.emulateCycle();
    int newProgramCounter = cpu.getProgramCounter();
    //each instruction moves the program counter by DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE. The skip instruction opcode moves the program counter by this amount,
    //so if the next instruction was skipped (as expected), the program counter should have moved by twice this amount.
    EXPECT_EQ(newProgramCounter, previousProgramCounter + Cpu::DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE + Cpu::DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE);
}

void expectInstructionNotSkipped(Cpu& cpu) {
    int previousProgramCounter = cpu.getProgramCounter();
    cpu.emulateCycle();
    int newProgramCounter = cpu.getProgramCounter();
    //if instruction executed as normal, the program counter will increment by the standard amount
    EXPECT_EQ(newProgramCounter, previousProgramCounter + Cpu::DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE);
}

//0x3XNN
TEST_F (CpuTestFixture, SkipEquals) {
    //Test to make sure the next instruction is skipped if the equality holds true
    uint8_t equalityValue = 0x10;
    uint16_t registerNumber = 0;
    uint16_t skipEqualsOpcode = (uint16_t)
            ((3 << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
            equalityValue);
    setRegister(memory, cpu, registerNumber, equalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipEqualsOpcode);
    expectInstructionSkipped(cpu);


    //Test to make sure the next instruction is not skipped if the equality is false
    //Note: the register being compared to inequalityValue is still set to equalityValue
    uint8_t inequalityValue = 0x11;
    uint16_t skipEqualsFailOpcode = (uint16_t)
            ((3 << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
            inequalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipEqualsFailOpcode);
    expectInstructionNotSkipped(cpu);
}

//0x4XNN
TEST_F (CpuTestFixture, SkipNotEquals) {
    //Test that the instruction is skipped when register value differs from opcode value
    uint8_t registerNumber = 10;
    uint8_t value1 = 0x10;
    uint8_t value2 = 0x11;
    uint16_t skipNotEqualsOpcode = (uint16_t) ((4 << OpcodeBitshifts::NIBBLE_THREE) |
                                            (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
                                            value1);
    setRegister(memory, cpu, registerNumber, value2);
    setOpcode(memory, cpu.getProgramCounter(), skipNotEqualsOpcode);
    expectInstructionSkipped(cpu);

    //Test that the instruction is not skipped when register value and opcode value are the same
    uint16_t skipNotEqualsFailOpcode = (uint16_t)
           ((4 << OpcodeBitshifts::NIBBLE_THREE) |
           (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
           value2);
    setOpcode(memory, cpu.getProgramCounter(), skipNotEqualsFailOpcode);
    expectInstructionNotSkipped(cpu);
}

//0x5XY0
TEST_F(CpuTestFixture, SkipRegisterEquals) {
    //Test that the instruction is skipped on register equality
    int register1 = 0;
    int register2 = 1;
    uint8_t equalityValue = 0x10;
    uint16_t skipRegisterEqualsOpcode = (uint16_t)
            ((5 << OpcodeBitshifts::NIBBLE_THREE) |
            (register1 << OpcodeBitshifts::NIBBLE_TWO) |
            (register2 << OpcodeBitshifts::NIBBLE));
    setRegister(memory, cpu, register1, equalityValue);
    setRegister(memory, cpu, register2, equalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipRegisterEqualsOpcode);
    expectInstructionSkipped(cpu);

    //Test that the instruction is not skipped when registers have differing values
    uint8_t inequalityValue = 0x11;
    setRegister(memory, cpu, register1, inequalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipRegisterEqualsOpcode);
    expectInstructionNotSkipped(cpu);
}

//0x6XNN
TEST_F(CpuTestFixture, SetRegister) {
    unsigned int registerNumber = 0;
    uint8_t expectedRegisterValue = 0x10;
    setRegister(memory, cpu, registerNumber, expectedRegisterValue);
    uint8_t retrievedRegisterValue = cpu.getRegisterValue(registerNumber);
    EXPECT_EQ(expectedRegisterValue, retrievedRegisterValue);
}

//0x7XNN
TEST_F(CpuTestFixture, AddRegister) {
    unsigned int registerNumber = 0;
    uint8_t originalRegisterValue = 1;
    uint8_t addedValue = 9;
    setRegister(memory, cpu, registerNumber, originalRegisterValue);
    uint16_t addRegisterOpcode = (uint16_t) ((7 << OpcodeBitshifts::NIBBLE_THREE) | (registerNumber << OpcodeBitshifts::NIBBLE_TWO) | addedValue);
    setOpcode(memory, cpu.getProgramCounter(), addRegisterOpcode);
    cpu.emulateCycle();
    EXPECT_EQ(originalRegisterValue + addedValue, cpu.getRegisterValue(registerNumber));
}

void executeOpcode(Memory& memory, Cpu& cpu, uint16_t opcode) {
    setOpcode(memory, cpu.getProgramCounter(), opcode);
    cpu.emulateCycle();
}

/**
 * sets two separate registers to valueX and valueY, generates and executes an arithmetic opcode,
 * then checks the value of the first register (registerX) for the expectedOutput
 * @param opcodeArithmeticOperationNumber is the last nibble of an arithmetic opcode (any opcode that begins with 8)
 * and is used to generate the arithmetic opcode tested against
 */
void testArithmeticOperator(Memory &memory, Cpu &cpu, uint8_t valueX, uint8_t valueY, uint8_t expectedOutput,
                            uint16_t opcodeArithmeticOperationNumber) {
    //registers X and Y are arbitrary and could be any valid register (0 to 15)
    unsigned int registerNumberX = 0;
    unsigned int registerNumberY = 1;
    setRegister(memory, cpu, registerNumberX, valueX);
    setRegister(memory, cpu, registerNumberY, valueY);
    uint16_t setRegisterOrOpcode = (uint16_t)
            ((8 << OpcodeBitshifts::NIBBLE_THREE) |
             (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
             (registerNumberY << OpcodeBitshifts::NIBBLE) | opcodeArithmeticOperationNumber);
    executeOpcode(memory, cpu, setRegisterOrOpcode);
    EXPECT_EQ(expectedOutput, cpu.getRegisterValue(registerNumberX));
}

//0x8XY0
TEST_F(CpuTestFixture, SetRegisterToRegister) {
    //note the original value of x doesn't matter for this opcode because it will be overwritten (if the opcode works correctly)
    uint8_t valueY = 10;
    uint8_t expectedOutput = 10;
    uint8_t opcodeArithmeticOperationNumber = 0;
    testArithmeticOperator(memory, cpu, 0, valueY, expectedOutput, opcodeArithmeticOperationNumber);
}

//0x8XY1
TEST_F(CpuTestFixture, setRegisterOr) {
    //the bits of valueX, valueY, and expectedOutput form a truth table of the OR operator
    //if you read the bits of valueX, valueY, and expectedOutput vertically.
    //0 | 0 = 0
    //0 | 1 = 1
    //1 | 0 = 1
    //1 | 1 = 1
    uint8_t valueX = 0b0011;
    uint8_t valueY = 0b0101;
    uint8_t expectedOutput = 0b0111;
    uint8_t opcodeArithmeticOperationNumber = 1;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
}

//0x8XY2
TEST_F(CpuTestFixture, setRegisterAnd) {
    //similar to setRegisterOr, valueX, valueY, and expectedOutput form a truth table of AND
    uint8_t valueX = 0b0011;
    uint8_t valueY = 0b0101;
    uint8_t expectedOutput = 0b0001;
    uint8_t opcodeArithmeticOperationNumber = 2;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
}

//0x8XY3
TEST_F(CpuTestFixture, setRegisterXor) {
    //similar to the previous bitwise operators, valueX, valueY, and expectedOutput form a truth table of XOR
    uint8_t valueX = 0b0011;
    uint8_t valueY = 0b0101;
    uint8_t expectedOutput = 0b0110;
    uint8_t opcodeArithmeticOperationNumber = 3;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
}

//0x8XY4
TEST_F(CpuTestFixture, addRegisterToRegister) {
    //Test regular operation
    uint8_t valueX = 10;
    uint8_t valueY = 11;
    uint8_t expectedOutput = valueX + valueY;
    uint8_t opcodeArithmeticOperationNumber = 4;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedCarryBit = 0;
    EXPECT_EQ(expectedCarryBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

TEST_F(CpuTestFixture, addRegisterToRegisterCarry) {
    uint8_t valueX = 255;
    uint8_t valueY = 255;
    uint8_t expectedOutput = valueX + valueY;
    uint8_t opcodeArithmeticOperationNumber = 4;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedCarryBit = 1;
    EXPECT_EQ(expectedCarryBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0x8XY5
TEST_F(CpuTestFixture, subtractRegisterFromRegister) {
    uint8_t valueX = 11;
    uint8_t valueY = 10;
    uint8_t expectedOutput = valueX - valueY;
    uint8_t opcodeArithmeticOperationNumber = 5;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedBorrowBit = 1;
    EXPECT_EQ(expectedBorrowBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

TEST_F(CpuTestFixture, subtractRegisterFromRegisterBorrow) {
    //Test when there's a borrow (
    uint8_t valueX = 10;
    uint8_t valueY = 11;
    uint8_t expectedOutput = valueX - valueY;
    uint8_t opcodeArithmeticOperationNumber = 5;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedBorrowBit = 0;
    EXPECT_EQ(expectedBorrowBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0x8XY6
TEST_F(CpuTestFixture, shiftRegisterRightZeroShiftedOut) {
    //note valueY is not used for this opcode
    uint8_t valueX = 0b1010;
    uint8_t expectedOutput = 0b101;
    uint8_t opcodeArithmeticOperationNumber = 6;
    testArithmeticOperator(memory, cpu, valueX, 0, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedShiftedOutBit = 0;
    EXPECT_EQ(expectedShiftedOutBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

TEST_F(CpuTestFixture, shiftRegisterRightOneShiftedOut) {
    uint8_t valueX = 0b1011;
    uint8_t expectedOutput = 0b101;
    uint8_t opcodeArithmeticOperationNumber = 6;
    testArithmeticOperator(memory, cpu, valueX, 0, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedShiftedOutBit = 1;
    EXPECT_EQ(expectedShiftedOutBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0x8XY7
TEST_F(CpuTestFixture, subtractReverseRegisterFromRegister) {
    uint8_t valueX = 10;
    uint8_t valueY = 11;
    uint8_t expectedOutput = 1;
    uint8_t opcodeArithmeticOperationNumber = 7;
    testArithmeticOperator(memory, cpu, valueX, valueY, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedBorrowBit = 1;
    EXPECT_EQ(expectedBorrowBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0x8XYE
TEST_F(CpuTestFixture, shiftRegisterLeftZeroShiftedOut) {
    //note valueY is not used for this opcode.
    uint8_t valueX = 0b101;
    uint8_t expectedOutput = 0b1010;
    uint8_t opcodeArithmeticOperationNumber = 0xE;
    testArithmeticOperator(memory, cpu, valueX, 0, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedShiftedOutBit = 0;
    EXPECT_EQ(expectedShiftedOutBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

TEST_F(CpuTestFixture, shiftRegisterLeftOneShiftedOut) {
    uint8_t valueX = 0b10101010;
    uint8_t expectedOutput = 0b01010100;
    uint8_t opcodeArithmeticOperationNumber = 0xE;
    testArithmeticOperator(memory, cpu, valueX, 0, expectedOutput, opcodeArithmeticOperationNumber);
    uint8_t expectedShiftedOutBit = 1;
    EXPECT_EQ(expectedShiftedOutBit, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}