#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "defs.h"
#include "levelman.h"
#include "widget.h"
#include "seeker.h"
#include "dialogue.h"

class Game {
public:
    Game();
    ~Game();
    bool init();

    // Main game loop
    void run();
    void input();
    void update(float deltaTime);
    void render();
    void controlFrameRate(Uint32 frameStart, int frameDelay);
    void handleEventPause();

    void close();
    void thegreatReset();
    SDL_Renderer* getRenderer() const { return gRenderer; }

private:
    bool quit = false;
    bool pauseGame = false;
    bool hasTrigger = false; //for dialogue
    bool hasFirstDialogue = false;
    bool hasSecondDialogue = false;
    Uint32 deathTimer = 0;
    bool waitKey = false;

    LevelManager* levelManager;
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;

    // The playable character
    Player* player;
    Seeker* seeker;
    DialogueManager dialogue;

    Widget menu; 
    Widget settingsMenu;
    Widget pauseMenu;

    SDL_Texture* far;
    SDL_Texture* mid;
    SDL_Texture* mid_near;
    SDL_Texture* near;

    int continueButton, quitButton, settings;
    int volumeButton, musicButton, backButton;
    int start_button, settings_button, exit_button;

    Mix_Music* music = nullptr;

    TTF_Font *font;
    SDL_Texture* loadTexture(const std::string &path);
};

extern Uint32 lastTime;
extern float deltaTime;

#endif 
