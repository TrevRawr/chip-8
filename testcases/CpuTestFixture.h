#ifndef CHIP_8_CPUTESTFIXTURE_H
#define CHIP_8_CPUTESTFIXTURE_H

#include "../src/cpu/Cpu.h"
#include "../src/storage/Memory.h"
#include "gtest/gtest.h"
#include "mocks/MockDisplay.h"
#include "mocks/MockInputController.h"

/**
 * A Test fixture for initializing a fresh cpu instance (along with its mocked out dependencies) for each test case
 * in order to achieve isolated unit tests.
 */
class CpuTestFixture : public ::testing::Test {
   protected:
    CpuTestFixture();

    Chip8::Memory memory;
    MockDisplay display;
    MockInputController inputController;
    Chip8::Cpu cpu;
};

#endif  // CHIP_8_CPUTESTFIXTURE_H
