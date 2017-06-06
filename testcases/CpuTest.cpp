#include "gtest/gtest.h"
#include "CpuTestFixture.h"
#include "../src/constants/Constants.h"
#include "../src/constants/OpcodeBitshifts.h"
#include "../src/exceptions/IndexOutOfBoundsException.h"

void setOpcode(Memory& memory, uint16_t address, uint16_t opcode) {
    memory.setDataAtAddress(address,
                             (uint8_t) ((opcode & OpcodeBitmasks::FIRST_BYTE) >> OpcodeBitshifts::NIBBLE_TWO));
    memory.setDataAtAddress(address + 1, (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE));
}

TEST_F(CpuTestFixture, ClearScreen) {
    uint16_t clearScreenOpcode = 0x00E0;
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, clearScreenOpcode);

    EXPECT_CALL(display, clearScreen());
    cpu.emulateCycle();
}

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

TEST_F (CpuTestFixture, JumpOpcode) {
    //expected address is random and insignificant
    uint16_t expectedAddress = 0x205;
    uint16_t jumpToAddressOpcode = (uint16_t) (0x1000 | expectedAddress);
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, jumpToAddressOpcode);
    cpu.emulateCycle();
    EXPECT_EQ(cpu.getProgramCounter(), expectedAddress);
};

void setRegister(Memory& memory, Cpu& cpu, int registerNumber, uint8_t registerValue) {
    uint16_t setRegisterOpcode = (uint16_t) ((6 << OpcodeBitshifts::NIBBLE_THREE) |
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

TEST_F (CpuTestFixture, SkipEquals) {
    //Test to make sure the next instruction is skipped if the equality holds true
    uint8_t equalityValue = 0x10;
    uint16_t registerNumber = 0;
    uint16_t skipEqualsOpcode = (uint16_t) ((3 << OpcodeBitshifts::NIBBLE_THREE) |
                                                 (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
                                                 equalityValue);
    setRegister(memory, cpu, registerNumber, equalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipEqualsOpcode);
    expectInstructionSkipped(cpu);


    //Test to make sure the next instruction is not skipped if the equality is false
    //Note: the register being compared to inequalityValue is still set to equalityValue
    uint8_t inequalityValue = 0x11;
    uint16_t skipEqualsFailOpcode = (uint16_t) ((3 << OpcodeBitshifts::NIBBLE_THREE) |
                                                     (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
                                                     inequalityValue);
    setOpcode(memory, cpu.getProgramCounter(), skipEqualsFailOpcode);
    expectInstructionNotSkipped(cpu);
}

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
    uint16_t skipNotEqualsFailOpcode = (uint16_t) ((4 << OpcodeBitshifts::NIBBLE_THREE) |
                                                   (registerNumber << OpcodeBitshifts::NIBBLE_TWO) |
                                                   value2);
    setOpcode(memory, cpu.getProgramCounter(), skipNotEqualsFailOpcode);
    expectInstructionNotSkipped(cpu);
}

TEST_F(CpuTestFixture, SkipRegisterEquals) {
    //Test that the instruction is skipped on register equality
    int register1 = 0;
    int register2 = 1;
    uint8_t equalityValue = 0x10;
    uint16_t skipRegisterEqualsOpcode = (uint16_t) ((5 << OpcodeBitshifts::NIBBLE_THREE) |
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

TEST_F(CpuTestFixture, SetRegister) {
    unsigned int registerNumber = 0;
    uint8_t expectedRegisterValue = 0x10;
    setRegister(memory, cpu, registerNumber, expectedRegisterValue);
    uint8_t retrievedRegisterValue = cpu.getRegisterValue(registerNumber);
    EXPECT_EQ(expectedRegisterValue, retrievedRegisterValue);
}

