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
                break;
            case SDLK_UP:
                if (curWid > 0) {
                    curWid--;
                }
                break;
            case SDLK_RETURN:
                std::cout << "Button pressed: " << buttons[curWid].label << "\n";
                if(buttons[curWid].label == "Start a new game"){
                    gameState = STATE_GAME;
                }
                else if(buttons[curWid].label == "Exit ..."){
                    gameState = STATE_END;
                }
                break;
        }
    }
}

void Widget::button_render(SDL_Renderer* renderer, TTF_Font *font) {
    for (const auto& button : buttons) {
        if (button.image.getTex()) {
            // Render button with image
            SDL_RenderCopy(renderer, button.image.getTex(), NULL, &button.rect);
        } else {
            // Render default gray rectangle if no texture is set
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &button.rect);
        }

        // Render button text
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