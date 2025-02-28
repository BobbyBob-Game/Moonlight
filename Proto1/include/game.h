#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED
#include <iostream>
#include <map>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>


#include "object.h"
#include "entity.h"
#include "widget.h"
#include "dia.h"
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

    void loadMap(const char *filename, int sizeX, int sizeY);
    void drawMap();
    void loadTileTexture(int dimensionID);

    void drawLetter(const char* msg, int x, int y, int r, int g, int b, int size);
    bool collision(Object a, Object b);

    private:
    int dimensionID;
    SDL_Renderer *renderer;
    SDL_Window* window;
    bool running;
    TTF_Font *font;
    Widget menu;
    Dia dialogueBox;

    int count;
    int frameCount, timerFPS, lastFrame;
    Uint32 lastFrameTime;

    Entity player;
    Object background;
    Object dia;
    Object gameTitle;

    vector<vector<int>> tileMap1;
    vector<vector<int>> tileMap2;
    map<int, SDL_Texture*> tileTextures1;
    map<int, SDL_Texture*> tileTextures2;

    int mapX, mapY;
    int scrollingSpeed;

    const int runningSpeedNormal = 6; //6 m/s

    bool left, right;
    bool fall = true;
    bool water = false;
    int idle_left, idle_right, run_left, run_right;

    int start_button;
    int exit_button;
};



#endif