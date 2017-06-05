#ifndef CHIP_8_CPUTESTFIXTURE_H
#define CHIP_8_CPUTESTFIXTURE_H

#include "gtest/gtest.h"
#include "../src/storage/Memory.h"
#include "../src/cpu/Cpu.h"
#include "mocks/MockDisplay.h"
#include "mocks/MockInputController.h"

class CpuTestFixture : public ::testing::Test {
protected:
    Memory * memory;
    MockDisplay* display;
    MockInputController* inputController;
    Cpu* cpu;
    void SetUp() override;

    void TearDown() override;
};


#endif //CHIP_8_CPUTESTFIXTURE_H
