#ifndef CHIP_8_CPUTESTFIXTURE_H
#define CHIP_8_CPUTESTFIXTURE_H

#include "gtest/gtest.h"
#include "../src/storage/Memory.h"
#include "../src/cpu/Cpu.h"
#include "mocks/MockDisplay.h"
#include "mocks/MockInputController.h"

using ::testing::NiceMock;

class CpuTestFixture : public ::testing::Test {
protected:
    CpuTestFixture();

protected:
    Memory memory;
    MockDisplay display;
    MockInputController inputController;
    Cpu cpu;
};


#endif //CHIP_8_CPUTESTFIXTURE_H
