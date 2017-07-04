#ifndef CHIP_8_ISUBSYSTEMMANAGER_H
#define CHIP_8_ISUBSYSTEMMANAGER_H

#include "display/IDisplay.h"
#include "input/IInputController.h"

/**
 * An interface for controlling initialization and destruction of a set of multimedia subsystems
 */
namespace Chip8 {
class ISubsystemManager {
   public:
    virtual IInputController &getInputController() = 0;

    virtual IDisplay &getDisplay() = 0;

    virtual ~ISubsystemManager(){};
};
}

#endif  // CHIP_8_ISUBSYSTEMMANAGER_H
