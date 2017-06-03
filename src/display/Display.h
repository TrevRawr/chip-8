#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL.h>

class Display {
public:
    static const int SPRITE_WIDTH = 8;

    Display();
    virtual ~Display();

    /**
     * sets a pixel at the specified coordinate to on or off. If the pixel is unspecified, does nothing.
     * updateScreen() must be called for this to take effect.
     */
    void setPixel(int x, int y, bool value);

    /**
     * @return true if the pixel is on, false otherwise. If the value of the pixel is out of screen bounds, return false
     * updateScreen() must be called for this to take effect.
     */
    bool getPixel(int x, int y);
    void clearScreen();

    /**
     * sets all the pixels on the screen that were set with setPixel()
     */
    void updateScreen();

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
