#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "player.h"
#include "defs.h"
#include "map.h"

class Game {
public:
    Game();
    ~Game();

    // Initializes SDL, window, renderer, etc.
    bool init();

    // Loads media for background and player
    bool loadMedia();
    void renderBackground(SDL_Renderer* renderer, SDL_Texture *gLayerX, float offsetX);

    // Main game loop
    void run();

    // Frees media and shuts down SDL
    void close();

    // Get the renderer (for use in other classes)
    SDL_Renderer* getRenderer() const { return gRenderer; }

private:
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;

    // Background textures for parallax
    SDL_Texture* gLayer1;
    SDL_Texture* gLayer2;
    SDL_Texture* gLayer3;
    SDL_Texture* gLayer4;

    // Offsets for parallax scrolling
    float offset1;
    float offset2;
    float offset3;
    float offset4;

    //Map:
    Map currentMap;    

    // Base speed for scrolling
    const float baseSpeed = 100.0f;

    // The playable character
    Player* player;

    // Helper: Load a texture from file
    SDL_Texture* loadTexture(const std::string &path);
};

#endif // GAME_H
