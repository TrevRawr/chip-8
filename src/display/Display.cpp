#include "Display.h"

void Display::setPixel(int x, int y, bool value) {
    if (isPixelInBounds(x, y)) {
        int pixelIndex = getPixelIndex(x, y);
        screenPixels[pixelIndex] = value;
        if (value) {
            setSdlPixel(x, y, 0xFFFF);
        } else {
            setSdlPixel(x, y, 0x0000);
        }
    }
}

bool Display::getPixel(int x, int y) {
    if (isPixelInBounds(x, y)) {
        return screenPixels[getPixelIndex(x, y)];
    }
    return false;
}

int Display::getPixelIndex(int x, int y) const {
    return x + SCREEN_WIDTH * y;
}

bool Display::isPixelInBounds(int x, int y) {
    return !(x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT);
}

Display::Display() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screenPixels[i] = false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
        surface = SDL_GetWindowSurface(window);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
        SDL_UpdateWindowSurface(window);
    }
}

void Display::setSdlPixel(int x, int y, uint32_t pixel) {
    Uint32 *pixels = (Uint32 *) surface->pixels;
    pixels[y * SCREEN_WIDTH + x] = pixel;
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::clearScreen() {
    for (int width = 0; width < SCREEN_WIDTH; width++) {
        for (int height = 0; height < SCREEN_HEIGHT; height++) {
            setPixel(width, height, false);
        }
    }
}

void Display::updateScreen() {
    SDL_UpdateWindowSurface(window);
}


