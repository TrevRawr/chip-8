#ifndef CHIP_8_IDISPLAY_H
#define CHIP_8_IDISPLAY_H

#include <SDL.h>

class IDisplay {
public:
    static const int SPRITE_WIDTH = 8;

    virtual ~IDisplay() {};

    /**
     * sets a pixel at the specified coordinate to on or off. If the pixel is unspecified, does nothing.
     * updateScreen() must be called for this to take effect.
     */
    virtual void setPixel(int x, int y, bool value) = 0;

    /**
     * @return true if the pixel is on, false otherwise. If the value of the pixel is out of screen bounds, return false
     * updateScreen() must be called for this to take effect.
     */
    virtual bool getPixel(int x, int y) = 0;

    virtual void clearScreen() = 0;

    /**
     * sets all the pixels on the screen that were set with setPixel()
     */
    virtual void updateScreen() = 0;
};


#endif //CHIP_8_IDISPLAY_H
