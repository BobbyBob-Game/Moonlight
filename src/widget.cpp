#include "widget.h"

int Widget::createWidget(int x, int y, int w, int h, std::string label){
    button temp;
    temp.rect = {x,y,w,h};
    temp.label = label;
    buttons.push_back(temp);
    return buttons.size() - 1;
}

void Widget::updateWidget(SDL_Event& event, GameState& gameState) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_DOWN:
                if (curWid < buttons.size() - 1) {
                    curWid++;
                }
                Mix_PlayChannel(-1, selectSound, 0);
                break;
            case SDLK_UP:
                if (curWid > 0) {
                    curWid--;
                }
                Mix_PlayChannel(-1, selectSound, 0);
                break;
            case SDLK_RETURN:
                std::cout << "Button pressed: " << buttons[curWid].label << "\n";
                Mix_PlayChannel(-1, buttonSound, 0);
                if(buttons[curWid].label == "Start"){
                    gameState = STATE_GAME;
                }
                else if(buttons[curWid].label == "Settings"){
                    prevState = gameState;
                    gameState = STATE_SETTINGS;
                }
                else if(buttons[curWid].label == "Quit"){
                    gameState = STATE_END;
                }
                else if(buttons[curWid].label == "Back"){
                    gameState = prevState;
                }
                else if(buttons[curWid].label == "Continue"){
                    gameState = STATE_GAME;
                }
                else if(buttons[curWid].label == "Music"){
                    if(Mix_PausedMusic()){
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                }
                break;
        }
    }
}

void Widget::button_render(SDL_Renderer* renderer, TTF_Font *font) {
    for (const auto& button : buttons) {
        if (button.image.getTex()) {
            SDL_RenderCopy(renderer, button.image.getTex(), NULL, &button.rect);
        }

        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurf = TTF_RenderText_Solid(font, button.label.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurf);
        
        SDL_Rect textRect = {
            button.rect.x + (button.rect.w - textSurf->w) / 2,
            button.rect.y + (button.rect.h - textSurf->h) / 2,
            textSurf->w, textSurf->h
        };

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurf);
        SDL_DestroyTexture(textTexture);

        if (arrow.getTex() && curWid >= 0 && curWid < buttons.size()) {
            const auto& selectedButton = buttons[curWid];
            SDL_Rect arrowRect = {
                selectedButton.rect.x - 70, 
                selectedButton.rect.y + (selectedButton.rect.h - 30) / 2,
                50, 40 
            };
            SDL_RenderCopy(renderer, arrow.getTex(), NULL, &arrowRect);
        }
    }
}

void Widget::setButtonTexture(int index, const std::string& filename, SDL_Renderer* renderer){
    buttons[index].image.setImage(filename, renderer);
}

void Widget::loadArrow(const std::string& filename, SDL_Renderer* renderer){
    arrow.setImage(filename, renderer);
}

void Widget::setBackground(const std::string& filename, SDL_Renderer* renderer){
    background.setImage(filename, renderer);
}

void Widget::updateBackground(float dt){ //scrolling
    backgroundX -= 50 * dt;
    if(backgroundX <= -SCREEN_WIDTH){
        backgroundX = 0;
    }
}

void Widget::renderBackground(SDL_Renderer* renderer){
    if(background.getTex()){
        SDL_Rect bgRect1 = {
            static_cast<int> (backgroundX),
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };

        SDL_Rect bgRect2 = {
            static_cast<int> (backgroundX + SCREEN_WIDTH),
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };

        SDL_RenderCopy(renderer, background.getTex(), NULL, &bgRect1);
        SDL_RenderCopy(renderer, background.getTex(), NULL, &bgRect2);
    }
}

bool Widget::loadSound(){
    bool success = true;
    buttonSound = Mix_LoadWAV("assets/sound/buttonSound.wav");
    if (!buttonSound) {
        std::cerr << "Failed to load button sound: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(buttonSound, MIX_MAX_VOLUME); 
    }

    selectSound = Mix_LoadWAV("assets/sound/selecting.wav");
    if(!selectSound){
        std::cerr << "Failed to load select sound: " << Mix_GetError() << "\n";
        success = false;
    }
    else{
        Mix_VolumeChunk(selectSound, MIX_MAX_VOLUME);
    }

    return success;
}

