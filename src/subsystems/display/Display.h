#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL.h>
#include "IDisplay.h"

/**
 * A (very) simple IDisplay implementation using SDL. Creates a window that is SCREEN_SCALE * the CHIP-8's physical screen resolution.
 * Ideally, this class should allow the user to specify an arbitrary scale, but the scale is currently hardcoded in SCREEN_SCALE.
 */
namespace Chip8 {
class Display : public IDisplay {
   public:
    Display();

    ~Display() override;

    void setPixel(int x, int y, bool value) override;

    bool getPixel(int x, int y) override;

    void clearScreen() override;

    void updateScreen() override;

   private:
    static const int SCREEN_SCALE = 10;
    static const int SCREEN_WIDTH = 64;
    static const int SCREEN_HEIGHT = 32;
    static const int PHYSICAL_SCREEN_WIDTH = SCREEN_WIDTH * SCREEN_SCALE;
    static const int PHYSICAL_SCREEN_HEIGHT = SCREEN_HEIGHT * SCREEN_SCALE;

    SDL_Surface *surface = NULL;
    SDL_Window *window = NULL;

    bool screenPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    bool isPixelInBounds(int x, int y);

    int getPixelIndex(int x, int y) const;

    void setSdlPixel(int x, int y, uint32_t pixel);
};
}

#endif  // CHIP_8_DISPLAY_H
