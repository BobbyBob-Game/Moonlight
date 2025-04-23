#ifndef ENDING_H
#define ENDING_H
#include "defs.h"

class CreditRenderer {
public:
    CreditRenderer(SDL_Renderer* ren, TTF_Font* f, int screenW, int screenH);
    ~CreditRenderer();
    void start(const std::vector<std::string>& lines);
    // Call every frame; returns false when done
    bool updateAndRender(float deltaTime);

private:
    SDL_Renderer* renderer;
    TTF_Font*     font;
    int           screenW, screenH;
    std::vector<SDL_Texture*> textures;
    std::vector<SDL_Rect>     rects;
    float         offsetY;
    const float   speed = 50.0f; // pixels per second
  
};

#endif