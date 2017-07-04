#include "CpuTestFixture.h"

CpuTestFixture::CpuTestFixture() : Test(), cpu(Chip8::Cpu(memory, display, inputController)) {}
