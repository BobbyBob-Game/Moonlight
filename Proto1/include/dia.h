#ifndef DIAGLOGUE_H_INCLUDED
#define DIAGLOGUE_H_INCLUDED
#include "object.h"
#include "defs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

class Dia {
public:
    void update(Uint32 currentTime);

    void render();

    SDL_Renderer* renderer;
    TTF_Font* font;
    std::string text = "Testing to see if the characters are drawn";
    int visibleChars;
    Uint32 lastUpdate;
};




#endif