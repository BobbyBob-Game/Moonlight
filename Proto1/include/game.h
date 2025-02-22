#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <fstream>

#include "object.h"
#include "entity.h"
using namespace std;

class Game {
    public:
    Game();
    ~Game();
    void loop();
    void input();
    void update();
    void render();
    void draw(Object o);
    void loadMap(const char *filename);
    void drawMap();
    bool collision(Object a, Object b);

    private:
    SDL_Renderer *renderer;
    SDL_Window* window;
    bool running;

    int count;
    int frameCount, timerFPS, lastFrame;

    Entity player;
    Object background;

    vector<Object> map;
    int mapX, mapY;
    int scrollingSpeed;

    const int runningSpeedNormal = 6;
    const double runningSpeedWater = 2.8;
    bool left, right, fall, water;
    int idle_left, idle_right, run_left, run_right;
};



#endif