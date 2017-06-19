#ifndef CHIP_8_CPUTESTFIXTURE_H
#define CHIP_8_CPUTESTFIXTURE_H

#include "gtest/gtest.h"
#include "../src/storage/Memory.h"
#include "../src/cpu/Cpu.h"
#include "mocks/MockDisplay.h"
#include "mocks/MockInputController.h"

/**
 * A Test fixture for initializing a fresh cpu instance (along with its mocked out dependencies) for each test case
 * in order to achieve isolated unit tests.
 */
class CpuTestFixture : public ::testing::Test {
protected:
    CpuTestFixture();

    Memory memory;
    MockDisplay display;
    MockInputController inputController;
    Cpu cpu;
};


#endif //CHIP_8_CPUTESTFIXTURE_H
