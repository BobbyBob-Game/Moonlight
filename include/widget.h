#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED
#include "defs.h"
#include "object.h"

using namespace std;

class Widget : public Object { 
    private:
    int curWid = 0;
    int begin = 0;
    Object arrow; 
    Object background;
    float backgroundX = 0;

    public:
    struct button {
        SDL_Rect rect;
        std::string label; //text displaying   
        Object image; 
    };
    
    std::vector<button> buttons;
    void loadArrow(const std::string& filename, SDL_Renderer* renderer);

    int createWidget(int x, int y, int w, int h, std::string label);
    void updateWidget(SDL_Event& event, GameState& gameState);
    void button_render(SDL_Renderer* renderer, TTF_Font* font);
    void setButtonTexture(int index, const std::string& filename, SDL_Renderer* renderer);
    void setBackground(const std::string& filename, SDL_Renderer* renderer);
    void updateBackground(float dt);
    void renderBackground(SDL_Renderer* renderer);
};


#endif