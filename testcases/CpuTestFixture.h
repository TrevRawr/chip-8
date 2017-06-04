#ifndef CHIP_8_CPUTESTFIXTURE_H
#define CHIP_8_CPUTESTFIXTURE_H

#include "gtest/gtest.h"
#include "../src/cpu/Cpu.h"

class CpuTestFixture : public ::testing::Test {
protected:
    Memory * memory;
    Display* display;
    InputController* inputController;
    Cpu* cpu;
    void SetUp() override;

    void TearDown() override;
};


#endif //CHIP_8_CPUTESTFIXTURE_H
