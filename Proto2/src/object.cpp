#include "object.h"

void Object::render(SDL_Renderer* destination, SDL_Rect* source){
    SDL_Rect src = {
        dest.x,
        dest.y,
        dest.w,
        dest.h
    };
    SDL_RenderCopy(destination, texture, source, &src);
}

void Object::render(int x, int y, SDL_Renderer* destination, SDL_Rect* source){
    SDL_Rect src = {x,y, dest.w, dest.h};
    if(!source) {src.w = source->x; src.h = source->h;}
    SDL_RenderCopy(destination, texture, source, &src);
}

void Object::setRect(int x, int y){
    dest.x = x;
    dest.y = y;
}

bool Object::loadImage(std::string filename, SDL_Renderer* renderer){
    SDL_Surface* overlay_surface = IMG_Load(filename.c_str());
    SDL_SetColorKey(overlay_surface, SDL_TRUE, SDL_MapRGB(overlay_surface->format, 255, 255, 255));
    SDL_Texture* overlay = SDL_CreateTextureFromSurface(renderer, overlay_surface);
    if(!overlay){dest.w = overlay_surface->w; dest.h = overlay_surface->h;}
    SDL_FreeSurface(overlay_surface);
    texture = overlay;
    return (texture != nullptr);
}

bool Object::loadText(std::string text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer){
    SDL_Surface* overlay_surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* overlay = SDL_CreateTextureFromSurface(renderer, overlay_surface);
    if(!overlay){dest.w = overlay_surface->w; dest.h = overlay_surface->h;}
    SDL_FreeSurface(overlay_surface);
    texture = overlay;
    return (texture != nullptr);
}

void Object::clear(){
    if(texture){
        SDL_DestroyTexture(texture);
        texture = nullptr;
        dest = {0,0,0,0};
    }
}