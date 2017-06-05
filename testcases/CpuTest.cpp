#include "gtest/gtest.h"
#include "CpuTestFixture.h"
#include "../src/constants/Constants.h"
#include "../src/constants/OpcodeBitshifts.h"

void setOpcode(Memory* memory, uint16_t address, uint16_t opcode) {
    memory->setDataAtAddress(address,
                             (uint8_t) ((opcode & OpcodeBitmasks::FIRST_BYTE) >> OpcodeBitshifts::BYTE_FIRST_TO_LAST));
    memory->setDataAtAddress(address + 1, (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE));
}

TEST_F(CpuTestFixture, ClearScreen) {
    uint16_t clearScreenOpcode = 0x00E0;
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, clearScreenOpcode);

    EXPECT_CALL(*display, clearScreen());
    cpu->emulateCycle();
}

TEST_F (CpuTestFixture, JumpOpcode) {
    uint16_t expectedAddress = 0x205;
    uint16_t jumpToAddressOpcode = (uint16_t) (0x1000 | expectedAddress);
    setOpcode(memory, Constants::MEMORY_PROGRAM_START_LOCATION, jumpToAddressOpcode);
    cpu->emulateCycle();
    EXPECT_EQ(cpu->getProgramCounter(), expectedAddress);
};