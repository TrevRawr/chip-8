#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL.h>
#include "IDisplay.h"

class Display : public IDisplay {
public:
    Display();

    ~Display() override;

    void setPixel(int x, int y, bool value) override;
    bool getPixel(int x, int y) override;
    void clearScreen() override;
    void updateScreen() override;

private:
    static const int SCREEN_WIDTH = 64;
    static const int SCREEN_HEIGHT = 32;

    SDL_Surface* surface = NULL;
    SDL_Window* window = NULL;

    bool screenPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    bool isPixelInBounds(int x, int y);

    int getPixelIndex(int x, int y) const;

    void setSdlPixel(int x, int y, uint32_t pixel);
};


#endif //CHIP_8_DISPLAY_H
