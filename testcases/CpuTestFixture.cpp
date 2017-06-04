#include "CpuTestFixture.h"

//TODO: use smart pointers?
void CpuTestFixture::SetUp() {
    memory = new Memory();
    display = new Display();
    inputController = new InputController();
    cpu = new Cpu(*memory, *display, *inputController);
}

void CpuTestFixture::TearDown() {
    delete cpu;
    delete inputController;
    delete display;
    delete memory;
}
