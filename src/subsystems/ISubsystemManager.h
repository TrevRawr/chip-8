#ifndef CHIP_8_ISUBSYSTEMMANAGER_H
#define CHIP_8_ISUBSYSTEMMANAGER_H


#include "input/IInputController.h"
#include "display/IDisplay.h"

/**
 * An interface for controlling initialization and destruction of a set of multimedia subsystems
 */
namespace Chip8 {
class ISubsystemManager {
public:
    virtual IInputController &getInputController() = 0;

    virtual IDisplay &getDisplay() = 0;

    virtual ~ISubsystemManager() {};
};
}

#endif //CHIP_8_ISUBSYSTEMMANAGER_H
