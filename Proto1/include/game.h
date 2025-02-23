#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <fstream>

#include "object.h"
#include "entity.h"
#include "widget.h"
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
    void drawLetter(const char* msg, int x, int y, int r, int g, int b, int size);
    bool collision(Object a, Object b);

    private:
    SDL_Renderer *renderer;
    SDL_Window* window;
    bool running;
    Widget *activeWidget;

    int count;
    int frameCount, timerFPS, lastFrame;

    Entity player;
    Object background;

    vector<Object> map;
    int mapX, mapY;
    int scrollingSpeed;

    const int runningSpeedNormal = 6; //6 m/s
    const double runningSpeedWater = 1.76; //1.11 m/s
    bool left, right;
    bool fall = true;
    bool water = false;
    bool isJumping = false;
    double UpVelocity = 0; //vertical speed of jump
    const double jumpForce = -30;
    const int GRAVITY = 9; //pixel based 
    int gravity_timer = 0;
    int idle_left, idle_right, run_left, run_right;
    const int JUMP_BUFFER_FRAME = 6;
    const int FLYING_TIME_FRAME = 8;
    const int MAX_FALL_SPEED = 7;
    const int JUMP_HOLD_TIMER = 12;
    const int JUMP_HOLD_FORCE = -1;
    int jumpHoldTimer = 0;
    int flyingTimer = 0;
    int jumpBuffer = 0;
};



#endif