#ifndef OBJECT_H
#define OBJECT_H
#include "defs.h"

class Object {
    public:
    SDL_Rect src, dest;
    void setRect(int x, int y);
    void setSource(int x, int y, int w, int h);
    void render(SDL_Renderer* destination, SDL_Rect* source);
    void render(int x, int y, SDL_Renderer* destination, SDL_Rect* source);
    void clear();
    int getDW() {return dest.w;}
    int getDH() {return dest.h;}

    SDL_Texture* getTex() const {return texture;}
    SDL_Rect getRect() const {return dest;}
    
    bool loadImage(std::string filename, SDL_Renderer* renderer);
    bool loadText(std::string text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer);
    void setImage(const std::string& filename, SDL_Renderer* renderer);
    private:
    SDL_Texture* texture;
};

class AnimatedObject : public Object { // this will be for small animation, the player animation is used by a different algorithm as it's easier to understand -Bach- 
    private:
    SDL_Rect src;         
    SDL_Rect dest;        
    SDL_Texture* text;    
    int mFrameCount;      
    int mFrameWidth;      
    int mFrameHeight;     
    Uint32 mFrameDelay;   
    int mCurrentFrame;    
    Uint32 mLastFrameTime;

    public:
    AnimatedObject(SDL_Renderer*& renderer, const std::string& filename, int frameCount, int frameWidth, int frameHeight, Uint32 frameDelay);
    ~AnimatedObject();
    void draw(int x, int y, int w, int h);
    void update();
    void render(SDL_Renderer*& renderer);
};

#endif