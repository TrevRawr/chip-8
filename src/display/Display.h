#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL.h>

class Display {
public:
    static const int SPRITE_WIDTH = 8;

    Display();
    virtual ~Display();

    void setPixel(int x, int y, bool value);
    bool getPixel(int x, int y);

private:
    static const int SCREEN_WIDTH = 64;
    static const int SCREEN_HEIGHT = 32;

    SDL_Surface* surface = NULL;
    SDL_Window* window = NULL;

    bool screenPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    void checkPixelInBounds(int x, int y);

    int getPixelIndex(int x, int y) const;

    void setSdlPixel(int x, int y, uint32_t pixel);
};


#endif //CHIP_8_DISPLAY_H
