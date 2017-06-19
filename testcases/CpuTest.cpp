#include "gtest/gtest.h"
#include "CpuTestFixture.h"
#include "../src/constants/Constants.h"
#include "../src/constants/OpcodeBitshifts.h"
#include "../src/exceptions/IndexOutOfBoundsException.h"
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

/**
 * A set of testcases for testing every chip-8 cpu opcode
 * Corresponding opcodes to testcases are commented on top of testcases. If a testcase doesn't have an opcode commented on it,
 * assume that it corresponds to the most recently read opcode comment
 */
void setOpcode(Memory& memory, uint16_t address, uint16_t opcode) {
    memory.setDataAtAddress(address, (uint8_t) ((opcode & OpcodeBitmasks::FIRST_BYTE) >> OpcodeBitshifts::NIBBLE_TWO));
    memory.setDataAtAddress(address + 1, (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE));
}

//0x00E0
TEST_F(CpuTestFixture, ClearScreen) {
    uint16_t clearScreenOpcode = 0x00E0;
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, clearScreenOpcode);

    EXPECT_CALL(display, clearScreen());
    EXPECT_CALL(display, updateScreen());
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
            (registerNumberY << OpcodeBitshifts::NIBBLE) |
            opcodeArithmeticOperationNumber);
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

//0x9XY0
TEST_F(CpuTestFixture, skipRegistersNotEqual) {
    //test instruction skipped
    unsigned int registerNumberX = 0;
    unsigned int registerNumberY = 1;
    uint8_t registerValueX = 1;
    uint8_t registerValueY = 0;
    setRegister(memory, cpu, registerNumberX, registerValueX);
    setRegister(memory, cpu, registerNumberY, registerValueY);
    uint16_t skipRegistersNotEqualOpcode = (uint16_t)
            ((9 << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            (registerNumberY << OpcodeBitshifts::NIBBLE));
    setOpcode(memory, cpu.getProgramCounter(), skipRegistersNotEqualOpcode);
    expectInstructionSkipped(cpu);

    //test instruction not skipped
    uint8_t newRegisterValueX = registerValueY;
    setRegister(memory, cpu, registerNumberX, newRegisterValueX);
    setOpcode(memory, cpu.getProgramCounter(), skipRegistersNotEqualOpcode);
    expectInstructionNotSkipped(cpu);
}

//0xANNN
TEST_F(CpuTestFixture, setIndexRegister) {
    uint16_t indexRegisterValue = 0xFFF;
    uint16_t setIndexRegisterOpcode = (uint16_t) ((0xA << OpcodeBitshifts::NIBBLE_THREE) | indexRegisterValue);
    executeOpcode(memory, cpu, setIndexRegisterOpcode);
    EXPECT_EQ(indexRegisterValue, cpu.getIndexRegisterValue());
}

//0xBNNN
TEST_F(CpuTestFixture, jumpToAddressPlusRegisterZero) {
    uint16_t baseAddress = 0x300;
    uint8_t registerValue = 0xFF;
    unsigned int registerNumber = 0;
    setRegister(memory, cpu, registerNumber, registerValue);
    uint16_t jumpToAddressPlusRegisterOpcode = (uint16_t) ((0xB << OpcodeBitshifts::NIBBLE_THREE) | baseAddress);
    executeOpcode(memory, cpu, jumpToAddressPlusRegisterOpcode);
    EXPECT_EQ(baseAddress + registerValue, cpu.getProgramCounter());

    //TODO: see what's supposed to happen if the opcode goes over the allowed address range. Is the program counter supposed to overflow and reset?
}

//0xCXNN
TEST_F(CpuTestFixture, randomNumber) {
    //TODO: research a good way to test functions with random numbers
    EXPECT_EQ(true, true);
}

//0xDXYN
TEST_F(CpuTestFixture, DrawSprite) {
    //5 is the sprite height of the default font loaded into memory
    unsigned int spriteHeight = 5;
    unsigned int spriteCoordinateX = 6;
    unsigned int spriteCoordinateY = 7;
    uint16_t drawSpriteOpcode = (uint16_t)
            ((0xD << OpcodeBitshifts::NIBBLE_THREE) |
            (spriteCoordinateX << OpcodeBitshifts::NIBBLE_TWO) |
            (spriteCoordinateY << OpcodeBitshifts::NIBBLE) |
            spriteHeight);
    uint16_t spriteLocation = Constants::MEMORY_FONT_START_LOCATION;
    uint16_t setIndexRegisterOpcode = (uint16_t) ((0xA << OpcodeBitshifts::NIBBLE_THREE) | spriteLocation);

    //set the index register to the location of the alphabet letter A sprite
    executeOpcode(memory, cpu, setIndexRegisterOpcode);

    //Check that the right amount of calls were made to setPixel()
    //TODO: assert more conditions such as that the correct sprite coordinates and values were set
    EXPECT_CALL(display, setPixel(_, _, _)).Times(spriteHeight * IDisplay::SPRITE_WIDTH);
    EXPECT_CALL(display, getPixel(_, _)).Times(AtLeast(1));
    EXPECT_CALL(display, updateScreen());
    executeOpcode(memory, cpu, drawSpriteOpcode);

    //Since the display was blank originally, no pixels should be switched off
    EXPECT_EQ(0, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0xEX9E
TEST_F(CpuTestFixture, skipOnKeyPressed) {
    unsigned int registerNumberX = 0;
    uint8_t buttonNumber = 0;
    uint16_t skipOnKeyPressedOpcode = (uint16_t)
            ((0xE << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x9E);
    setRegister(memory, cpu, registerNumberX, buttonNumber);

    //should return true for first call (in expectInstructionSkipped)
    //and false for the second call (in expectInstructionNotSkipped)
    EXPECT_CALL(inputController, isKeyPressed(buttonNumber))
            .WillOnce(Return(true))
            .WillOnce(Return(false));

    setOpcode(memory, cpu.getProgramCounter(), skipOnKeyPressedOpcode);
    expectInstructionSkipped(cpu);

    setOpcode(memory, cpu.getProgramCounter(), skipOnKeyPressedOpcode);
    expectInstructionNotSkipped(cpu);
}

//0xEXA1
TEST_F(CpuTestFixture, skipOnKeyNotPressed) {
    unsigned int registerNumberX = 0;
    uint8_t buttonNumber = 0;
    uint16_t skipOnKeyNotPressedOpcode = (uint16_t)
            ((0xE << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            (0xA1));
    setRegister(memory, cpu, registerNumberX, buttonNumber);

    EXPECT_CALL(inputController, isKeyPressed(buttonNumber))
                .WillOnce(Return(false))
                .WillOnce(Return(true));

    setOpcode(memory, cpu.getProgramCounter(), skipOnKeyNotPressedOpcode);
    expectInstructionSkipped(cpu);

    setOpcode(memory, cpu.getProgramCounter(), skipOnKeyNotPressedOpcode);
    expectInstructionNotSkipped(cpu);
}

//0xFX07
TEST_F(CpuTestFixture, setRegisterToDelayTimer) {
    unsigned int registerNumberX = 0;
    uint8_t delayTimerValue = 0;
    uint16_t setRegisterToDelayTimerOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x07);
    executeOpcode(memory, cpu, setRegisterToDelayTimerOpcode);
    EXPECT_EQ(delayTimerValue, cpu.getRegisterValue(registerNumberX));
}

//0xFX0A
TEST_F(CpuTestFixture, waitForKeyPress) {
    unsigned int registerNumberX = 0;
    uint8_t keyPressed = 5;
    EXPECT_CALL(inputController, waitForKeyPress())
            .WillOnce(Return(keyPressed));
    uint16_t waitForKeyPressOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x0A);
    executeOpcode(memory, cpu, waitForKeyPressOpcode);
    EXPECT_EQ(keyPressed, cpu.getRegisterValue(registerNumberX));
}

//0xFX15
TEST_F(CpuTestFixture, setDelayTimer) {
    unsigned int registerNumberX = 0;
    uint8_t delay = 0x80;
    uint16_t setDelayTimerOpcode =
            (uint16_t) ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x15);
    setRegister(memory, cpu, registerNumberX, delay);
    executeOpcode(memory, cpu, setDelayTimerOpcode);
    EXPECT_EQ(delay, cpu.getDelayTimerValue());
}

//0xFX18
TEST_F(CpuTestFixture, setSoundTimer) {
    unsigned int registerNumberX = 0;
    uint8_t sound = 0x90;
    uint16_t setSoundTimerOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x18);
    setRegister(memory, cpu, registerNumberX, sound);
    executeOpcode(memory, cpu, setSoundTimerOpcode);
    EXPECT_EQ(sound, cpu.getSoundTimerValue());
}

//0xFX1E
TEST_F(CpuTestFixture, incrementIndexRegister) {
    unsigned int registerNumberX = 0;
    uint8_t incrementAmount = 255;
    uint16_t incrementIndexRegisterOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            0x1E);
    setRegister(memory, cpu, registerNumberX, incrementAmount);

    //increment once
    executeOpcode(memory, cpu, incrementIndexRegisterOpcode);
    EXPECT_EQ(incrementAmount, cpu.getIndexRegisterValue());
    EXPECT_EQ(0, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));

    //increment once again to ensure it's incrementing and not just setting the value of the register
    executeOpcode(memory, cpu, incrementIndexRegisterOpcode);
    EXPECT_EQ(incrementAmount + incrementAmount, cpu.getIndexRegisterValue());
    EXPECT_EQ(0, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

TEST_F(CpuTestFixture, incrementIndexRegisterOverflow) {
    uint16_t maxIndexRegisterValue = 0xFFF;
    uint16_t setIndexRegisterOpcode = (uint16_t) ((0xA << OpcodeBitshifts::NIBBLE_THREE) | maxIndexRegisterValue);
    executeOpcode(memory, cpu, setIndexRegisterOpcode);

    unsigned int registerNumberX = 0;
    uint8_t incrementAmount = 10;
    setRegister(memory, cpu, registerNumberX, incrementAmount);

    uint16_t incrementIndexRegisterOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
             (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
             0x1E);
    executeOpcode(memory, cpu, incrementIndexRegisterOpcode);
    EXPECT_EQ(incrementAmount - 1, cpu.getIndexRegisterValue());
    EXPECT_EQ(1, cpu.getRegisterValue(Cpu::INDEX_CARRY_REGISTER));
}

//0xFX29
TEST_F(CpuTestFixture, setIndexToSpriteLocation) {
    int registerNumberX = 0;
    int maxEightBitIntValue = 255;
    for (uint8_t i = 0; i < maxEightBitIntValue; i++) {
        uint16_t setIndexToSpriteLocationOpcode = (uint16_t)
                ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
                (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
                0x29);
        setRegister(memory, cpu, registerNumberX, i);
        executeOpcode(memory, cpu, setIndexToSpriteLocationOpcode);
        EXPECT_EQ(Constants::MEMORY_FONT_START_LOCATION + i * Constants::FONT_NUM_BYTES_PER_CHARACTER, cpu.getIndexRegisterValue());
    }
}

//0xFX33
TEST_F(CpuTestFixture, convertToBcd) {
    int registerNumberX = 0;
    uint8_t registerValue = 255;
    uint8_t expectedDigit1 = 2;
    uint8_t expectedDigit2 = 5;
    uint8_t expectedDigit3 = 5;
    setRegister(memory, cpu, registerNumberX, registerValue);
    uint16_t convertToBcdOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
            (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
            (0x33));
    executeOpcode(memory, cpu, convertToBcdOpcode);
    EXPECT_EQ(expectedDigit1, memory.getDataAtAddress(cpu.getIndexRegisterValue()));
    EXPECT_EQ(expectedDigit2, memory.getDataAtAddress(cpu.getIndexRegisterValue() + 1));
    EXPECT_EQ(expectedDigit3, memory.getDataAtAddress(cpu.getIndexRegisterValue() + 2));
}

void testRegisterDump(Memory& memory, Cpu& cpu, unsigned int numRegisters) {
    for (uint8_t registerNumber = 0; registerNumber < numRegisters; registerNumber++) {
        //store numbers equal to the number of the register in each register
        setRegister(memory, cpu, registerNumber, registerNumber);
    }
    int registerNumberX = numRegisters - 1;
    uint16_t registerDumpOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
             (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
             0x55);
    executeOpcode(memory, cpu, registerDumpOpcode);

    //check all memory addresses corresponding to all registers even if numRegisters is less than the number of registers
    //in the cpu. This way, the test verifies that addresses that shouldn't have been dumped are not dumped.
    for (unsigned int i = 0; i < Cpu::NUM_GENERAL_PURPOSE_REGISTERS; i++) {
        if (i < numRegisters) {
            EXPECT_EQ(i, memory.getDataAtAddress(cpu.getIndexRegisterValue() + i));
        } else {
            //this assumes the value in memory was zeroed out before executing
            EXPECT_EQ(0, memory.getDataAtAddress(cpu.getIndexRegisterValue() + i));
        }
    }
}

//0xFX55
TEST_F(CpuTestFixture, registerDump) {
    //zero out the memory addresses relevant to this test to ensure the tests work properly
    for (unsigned int i = 0; i < Cpu::NUM_GENERAL_PURPOSE_REGISTERS; i++) {
        memory.setDataAtAddress(cpu.getIndexRegisterValue() + i, 0);
    }
    //test dumping 1 register then 2 registers and so on up until dumping all registers
    //ideally, these should be separate test cases to reset memory between iterations in case the memory ever gets some
    //weird values set in it from one iteration, but it's easier to implement like this, and the scope of the test isn't that large
    for (unsigned int numRegisters = 1; numRegisters < Cpu::NUM_GENERAL_PURPOSE_REGISTERS; numRegisters++) {
        testRegisterDump(memory, cpu, numRegisters);
    }
}

void testRegisterLoad(Memory& memory, Cpu& cpu, unsigned int numRegisters) {
    for (unsigned int memoryValue = 0; memoryValue < numRegisters; memoryValue++) {
        memory.setDataAtAddress(cpu.getIndexRegisterValue() + memoryValue, (uint8_t) memoryValue);
    }

    int registerNumberX = numRegisters - 1;
    uint16_t registerLoadOpcode = (uint16_t)
            ((0xF << OpcodeBitshifts::NIBBLE_THREE) |
             (registerNumberX << OpcodeBitshifts::NIBBLE_TWO) |
             0x65);
    executeOpcode(memory, cpu, registerLoadOpcode);

    //check all registers even if numRegisters is less than the number of registers in the cpu to ensure that registers
    //that shouldn't be loaded are not loaded
    for (unsigned int i = 0; i < Cpu::NUM_GENERAL_PURPOSE_REGISTERS; i++) {
        if (i < numRegisters) {
            EXPECT_EQ(i, cpu.getRegisterValue(i));
        } else {
            //this assumes the original value in registers is set to zero
            EXPECT_EQ(0, cpu.getRegisterValue(i));
        }
    }
}

//0xFX65
TEST_F(CpuTestFixture, registerLoad) {
    //similar to registerDump, these should probably be separate test cases to reset registers and memory between test iterations,
    //but it's easier to implement this way and okay for this small test's scope
    for (unsigned int numRegisters = 1; numRegisters < Cpu::NUM_GENERAL_PURPOSE_REGISTERS; numRegisters++) {
        testRegisterLoad(memory, cpu, numRegisters);
    }
}