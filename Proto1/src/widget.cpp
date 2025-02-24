#include "widget.h"

int Widget::createWidget(int x, int y, int w, int h, std::string label){
    button temp;
    temp.rect = {x,y,w,h};
    temp.label = label;
    buttons.push_back(temp);
    return buttons.size() - 1;
}

void Widget::updateWidget(SDL_Event& event){
    if(event.type == SDL_KEYDOWN){
        switch (event.key.keysym.sym){
            case SDLK_DOWN:
                curWid = (curWid+1) % buttons.size();
                break;
            case SDLK_UP:
                curWid = (curWid-1 + buttons.size()) % buttons.size();
                break;
            case SDLK_RETURN:
                cout<<"Button pressed:  "<<buttons[curWid].label<<"\n";
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
    }
}


void Widget::setButtonTexture(int index, const std::string& filename, SDL_Renderer* renderer){
    buttons[index].image.setImage(filename, renderer);
}
