#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED
#include "defs.h"
#include "object.h"
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Widget : public Object { 
    private:
    int curWid;
    int begin = 0;

    struct button {
        SDL_Rect rect;
        std::string label; //text displaying   
        Object image; 
    };
    vector<button> buttons;

    public:
    Widget() : curWid(-1) {}
    int createWidget(int x, int y, int w, int h, std::string label);
    void updateWidget(SDL_Event& event);
    void button_render(SDL_Renderer* renderer, TTF_Font* font);
    void setButtonTexture(int index, const std::string& filename, SDL_Renderer* renderer);
};


#endif