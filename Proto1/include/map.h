#ifndef MAP_H_INLCUDED
#define MAP_H_INCLUDED

#include "defs.h"
#include "object.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> 

class Map {
public:
    // Constructors
    Map(int dimensionID, SDL_Renderer* renderer);
    ~Map();

    // Map operations
    void loadMap(const char* filename, int sizeX, int sizeY);
    void drawMap();

    // Texture loading for tiles based on dimension
    void loadTileTexture();
    bool isSolidTile(int tileID, int dimensionID);

    // Accessor to current tile map
    std::vector<std::vector<int>>& getCurrentTileMap();

    void tileCollision(class Object &obj, float &velX, float &velY, bool &onGround);

    void clampObjectPosition(Object &obj, float &velX, float &velY, bool &onGround);

private:
    int dimensionID;
    SDL_Renderer* renderer;

    // Two maps for different dimensions. Depending on your design, you may only need one map.
    std::vector<std::vector<int>> tileMap1;
    std::vector<std::vector<int>> tileMap2;

    // Tile textures for each dimension
    std::unordered_map<int, SDL_Texture*> tileTextures1;
    std::unordered_map<int, SDL_Texture*> tileTextures2;
};

#endif // MAP_H
