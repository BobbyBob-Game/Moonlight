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
    Dia(SDL_Renderer* renderer, TTF_Font* font)
    : renderer(renderer), font(font), text("Testing to see if the characters are drawn"),
    visibleChars(0), lastUpdate(SDL_GetTicks()) {}
    void update(Uint32 currentTime);

    void render();

    void startDialogue(const std::string& newText);

    SDL_Renderer* renderer;
    TTF_Font* font;
    std::string text;
    int visibleChars;
    Uint32 lastUpdate;
};




#endif