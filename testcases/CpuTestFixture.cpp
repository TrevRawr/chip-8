#include "CpuTestFixture.h"

CpuTestFixture::CpuTestFixture() : Test(), cpu(Cpu(memory, display, inputController)) {}
