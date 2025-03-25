#ifndef MAP_H
#define MAP_H
#include "defs.h"

class Map {
    public:
    Map(SDL_Renderer* renderer);
    void loadMap(const char* filename, int sizeX, int sizeY);
    void drawMap();
    void loadTileTexture();
    void clear();
    

    private:
    SDL_Renderer* renderer;
    std::vector<std::vector<int>> tileMap;
    std::unordered_map<int, SDL_Texture*> tileTexture;
};


#endif