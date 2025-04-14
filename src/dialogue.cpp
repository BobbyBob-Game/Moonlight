#include "dialogue.h"

void DialogueManager::trigger(const std::vector<std::string>& msg, bool showChoice){
    messages = msg;
    messageIndex = 0;
    displayedText = "";
    charIndex = 0;
    timer = 0.0f;
    active = true;
    fullyDisplayed = false;
    awaitingChoice = showChoice;
}

void DialogueManager::update(float deltaTime){
    if(!active || fullyDisplayed) return;
    timer += deltaTime;
    while(timer >= typeSpeed && charIndex < messages[messageIndex].size()){
        displayedText += messages[messageIndex][charIndex++];
        timer -= typeSpeed;
    }
    if(charIndex >= messages[messageIndex].size()){
        fullyDisplayed = true; //basically resetting after the whole sentence is displayed
    }
}

void DialogueManager::skipOrNext(){
    if(!active) return;
    if(!fullyDisplayed){
        displayedText = messages[messageIndex];
        charIndex = messages[messageIndex].size();
        fullyDisplayed = true;
    }
    else{
        if(messageIndex < messages.size() - 1){ 
            messageIndex++;
            displayedText = "";
            charIndex = 0;
            timer = 0.0f;
            fullyDisplayed = false;
        }
        else{
            active = false;
        }
    }
}

void DialogueManager::handleKey(SDL_Keycode key) {
    if (!active || !fullyDisplayed || !awaitingChoice || messageIndex != messages.size() - 2) return;

    if (key == SDLK_y) {
        mode = true;  
        active = false;
    } else if (key == SDLK_n) {
        mode = false;
        active = false;
    }
}

void DialogueManager::render(SDL_Renderer* renderer, TTF_Font* font){
    if(!active || !font || displayedText.empty()) return;

    SDL_Rect box = {100, 200, 500, 300};
    SDL_SetRenderDrawColor(renderer, 0,0,0, 200); //make it like the RPG boi
    SDL_RenderFillRect(renderer, &box);

    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, displayedText.c_str(), color, box.w - 20);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended_Wrapped failed: " << TTF_GetError() << "\n";
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect textRect = {box.x + 10, box.y + 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    if (fullyDisplayed && awaitingChoice && messageIndex == messages.size() - 2) {
        SDL_Surface* optSurf = TTF_RenderText_Blended(font, "Yes (Y) / No (N)", color);
        SDL_Texture* optTex = SDL_CreateTextureFromSurface(renderer, optSurf);
        SDL_Rect optRect = { box.x + 10, box.y + box.h - 30, optSurf->w, optSurf->h };
        SDL_RenderCopy(renderer, optTex, nullptr, &optRect);
        SDL_FreeSurface(optSurf);
        SDL_DestroyTexture(optTex);
    }
}