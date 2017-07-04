#ifndef CHIP_8_SDLSUBSYSTEMMANAGER_H
#define CHIP_8_SDLSUBSYSTEMMANAGER_H

#include <memory>
#include "ISubsystemManager.h"

/**
 * A class that controls the initialization and destruction of subsystems implemented with the SDL library
 */
namespace Chip8 {
class SdlSubsystemManager : public ISubsystemManager {
   public:
    SdlSubsystemManager();

    IInputController &getInputController() override;

    IDisplay &getDisplay() override;

    virtual ~SdlSubsystemManager();

   private:
    std::unique_ptr<IDisplay> display;
    std::unique_ptr<IInputController> inputController;
};
}

#endif  // CHIP_8_SDLSUBSYSTEMMANAGER_H
