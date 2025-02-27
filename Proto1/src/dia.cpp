#include "dia.h"

void Dia::render() {
    if (visibleChars == 0) return;

    std::string visibleText = text.substr(0, visibleChars);
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, visibleText.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {SCREEN_WIDTH / 2 - 335 ,25, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Dia::update(Uint32 currentTime) {
    if (currentTime > lastUpdate + 80) { 
        if (visibleChars < text.length()) {
            visibleChars++;
        }
        lastUpdate = currentTime;
    }
}