#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "defs.h"
#include "levelman.h"
#include "widget.h"

class Game {
public:
    Game();
    ~Game();

    // Initializes SDL, window, renderer, etc.
    bool init();

    // Loads media for background and player
    bool loadMedia();
    void renderBackground(SDL_Renderer* renderer, SDL_Texture* gLayerX, float offsetX, float offsetY, bool isVertical);
    void fade(int duration);
    void renderLevel();
    void renderPauseMenu(SDL_Renderer* renderer);

    // Main game loop
    void run();
    void input();
    void update(float deltaTime);
    void render();
    void controlFrameRate(Uint32 frameStart, int frameDelay);
    void handleEventPause();

    // Frees media and shuts down SDL
    void close();

    // Get the renderer (for use in other classes)
    SDL_Renderer* getRenderer() const { return gRenderer; }

private:
    bool quit = false;
    bool pauseGame = false;
    LevelManager* levelManager;
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

    // Base speed for scrolling
    const float baseSpeed = 100.0f;

    // The playable character
    Player* player;

    Widget menu; //Widget-related here:
    int start_button, exit_button;

    TTF_Font *font;

    // Helper: Load a texture from file
    SDL_Texture* loadTexture(const std::string &path);
};

extern Uint32 lastTime;
extern float deltaTime;

#endif // GAME_H
