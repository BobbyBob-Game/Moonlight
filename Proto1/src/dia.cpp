#include "dia.h"

void Dia::update(Uint32 currentTime) {
    // Reveal one more character every 60 ms
    if (currentTime > lastUpdate + 60) { 
        if (visibleChars < text.length()) {
            visibleChars++;
        }
        lastUpdate = currentTime;
    }
}

// Render the visible portion of the dialogue text
void Dia::render() {
    if (visibleChars == 0) return;

    std::string visibleText = text.substr(0, visibleChars);
    SDL_Color color = { 255, 255, 255, 255 };

    // Define the maximum width of the dialogue box (in pixels)
    int dialogueBoxWidth = 600;

    // Use TTF_RenderText_Blended_Wrapped to automatically wrap text
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, visibleText.c_str(), color, dialogueBoxWidth);
    if (!surface) {
        std::cerr << "Error creating surface: " << TTF_GetError() << "\n";
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface);
        return;
    }

    // Adjust the destination rectangle as needed; here we center the dialogue box horizontally
    SDL_Rect rect = { SCREEN_WIDTH / 2 - dialogueBoxWidth / 2, 25, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void Dia::startDialogue(const std::string& newText) {
    text = newText;
    visibleChars = 0;
    lastUpdate = SDL_GetTicks();
}