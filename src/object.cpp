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

void Object::setSource(int x, int y, int w, int h) {
    src.x = x;
    src.y = y;
    src.w = w;
    src.h = h;
}

void Object::clear(){
    if(texture){
        SDL_DestroyTexture(texture);
        texture = nullptr;
        dest = {0,0,0,0};
    }
}

void Object::setImage(const std::string& filename, SDL_Renderer* renderer){
    texture = IMG_LoadTexture(renderer, filename.c_str());
    if(!texture){
        std::cerr << "Failed to create texture from " << filename << ": " << SDL_GetError() << "\n";
    }
    else{
        std::cerr << "Successfully created texture from " << filename << "\n";
    }
}

AnimatedObject::AnimatedObject(SDL_Renderer*& renderer, const std::string& filename, int frameCount, int frameWidth, int frameHeight, Uint32 frameDelay)
    : mFrameCount(frameCount), mFrameWidth(frameHeight), mFrameHeight(frameHeight), mFrameDelay(frameDelay),
    mCurrentFrame(0), mLastFrameTime(0)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        text = nullptr;
    } else {
        text = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    src = {0,0,mFrameWidth, mFrameHeight};
}

AnimatedObject::~AnimatedObject(){
    if(text){
        SDL_DestroyTexture(text);
    }
}

void AnimatedObject::draw(int x, int y, int w, int h){
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
}

void AnimatedObject::update(){
    Uint32 currentTime = SDL_GetTicks();
    if(currentTime - mLastFrameTime >= mFrameDelay){
        mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
        src.x = mFrameWidth * mCurrentFrame;
        src.y = 0;
        src.w = mFrameHeight;
        src.h = mFrameHeight;
    }
}

void AnimatedObject::render(SDL_Renderer*& renderer){
    if(text){
        SDL_RenderCopy(renderer, text, &src, &dest);
    }
    else{
        std::cout<<"Cannot render texture of this animation\n";
    }
}