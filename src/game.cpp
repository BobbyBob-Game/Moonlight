#include "game.h"

GameState gameState = STATE_MENU;
Game::Game()
    : gWindow(nullptr), gRenderer(nullptr), gLayer1(nullptr), gLayer2(nullptr), gLayer3(nullptr),
      offset1(0.0f), offset2(0.0f), offset3(0.0f), offset4(0.0f), player(nullptr), pauseGame(false){
}

Game::~Game() {
    close();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return false;
    }
    gWindow = SDL_CreateWindow("Moonlight",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }
    
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont("assets/Font/8bitOperatorPlus8-Regular.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << "\n";
        return false;
    }

    menu.setBackground("assets/1.png", gRenderer);
    start_button = menu.createWidget(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 16, "Start");
    exit_button = menu.createWidget(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 16, "Quit");
    menu.setButtonTexture(start_button, "assets/Buttons/button.png", gRenderer);
    menu.setButtonTexture(exit_button,  "assets/Buttons/button.png", gRenderer);
    menu.loadArrow("assets/Buttons/select_button.png", gRenderer);    

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
        return false;
    }

    // Create the playable character
    player = new Player(
        gRenderer,    // SDL_Renderer*
        50.0f,        // posX (Starting X position)
        300.0f,       // posY (Starting Y position)
        10,            // normalSpeed
        20,           // dashSpeed
        0.0f,         // velY
        false,        // isJumping
        false,        // isFalling
        false,        // isWallSliding
        0,            // dashStartTime
        0             // dashCooldownTime
    );

    if (!player->loadSound()){
        std::cerr << "Failed to load player sound effects!\n";
        return false;
    }

    if (!loadMedia()) {
        std::cerr << "Failed to load background textures!\n";
        return false;
    }
    levelManager = new LevelManager(gRenderer);

    return true;
}

SDL_Texture* Game::loadTexture(const std::string &path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << "\n";
        return nullptr;
    }
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (!newTexture) {
        std::cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << "\n";
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

bool Game::loadMedia() {
    gLayer1 = loadTexture("assets/parallax/layer1.png");
    gLayer2 = loadTexture("assets/parallax/layer2.png");
    gLayer3 = loadTexture("assets/parallax/layer3.png");
    gLayer4 = loadTexture("assets/parallax/layer4.png");
    
    if (!gLayer1 || !gLayer2 || !gLayer3 || !gLayer4) {
        return false;
    }

    return true;
}

void Game::input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            pauseGame = !pauseGame;  
            std::cout << (pauseGame ? "Game Paused\n" : "Game Resumed\n");
            if(pauseGame && player){
                player->stopMovement();
            }
        }

        if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r){
            if(player){
                player->reset(false, levelManager->getCurrentLevel());
            }
        }
        if (!pauseGame && player) { 
            player->handleEvent(event, pauseGame);
        }
    }
}

void Game::update(float deltaTime) {
    if(pauseGame) return;
    bool isVertical = levelManager->isSpecialLevel();

    // Get display size
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int screenWidth = DM.w;
    int screenHeight = DM.h;

    int newWidth, newHeight;

    if (isVertical) {
        newWidth = SCREEN_HEIGHT;
        newHeight = SCREEN_WIDTH;
    } else {
        newWidth = SCREEN_WIDTH;
        newHeight = SCREEN_HEIGHT;
    }

    // Resize the window
    SDL_SetWindowSize(gWindow, newWidth, newHeight);

    // Center the window
    int newX = (screenWidth - newWidth) / 2;
    int newY = (screenHeight - newHeight) / 2;
    SDL_SetWindowPosition(gWindow, newX, newY);

    // Update the player
    player->update(deltaTime, pauseGame);
    player->updateAnimation(deltaTime);
    levelManager->updateCheckpoint(deltaTime, *player);

    // Handle level transition
    if (player->reachedExit()) {
        levelManager->fade(gRenderer, 500, false);
        levelManager->NextLevel();
        player->reset(true, levelManager->getCurrentLevel());
    }

    // Update background offsets
    offset1 -= baseSpeed * 0.2f * deltaTime;
    offset2 -= baseSpeed * 0.5f * deltaTime;
    offset3 -= baseSpeed * 1.0f * deltaTime;
    offset4 -= baseSpeed * 1.2f * deltaTime;

    if (offset1 <= -newWidth) offset1 = 0;
    if (offset2 <= -newWidth) offset2 = 0;
    if (offset3 <= -newWidth) offset3 = 0;
    if (offset4 <= -newWidth) offset4 = 0;
}


void Game::render() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);
    if(gameState == STATE_MENU){
        menu.renderBackground(gRenderer);
        //return; not yet 
    }

    bool isVertical = levelManager->isSpecialLevel();

    /*if (!isVertical) {
        // Horizontal scrolling (X-axis)
        renderBackground(gRenderer, gLayer1, offset1, 0, false);
        renderBackground(gRenderer, gLayer2, offset2, 0, false);
        renderBackground(gRenderer, gLayer3, offset3, 0, false);
        renderBackground(gRenderer, gLayer4, offset4, 0, false);
    } else {
        // Vertical scrolling (Y-axis)
        renderBackground(gRenderer, gLayer1, 0, offset1, true);
        renderBackground(gRenderer, gLayer2, 0, offset2, true);
        renderBackground(gRenderer, gLayer3, 0, offset3, true);
        renderBackground(gRenderer, gLayer4, 0, offset4, true);
    }*/

    // Render the tile map and player
    std::vector<std::vector<Tile>> grid = levelManager->GetLevelData();
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid[y].size(); x++) {
            SDL_Texture* tileTex = levelManager->GetTexture(grid[y][x].type);
            if (tileTex) {
                SDL_Rect dstRect;
                if (!isVertical) {
                    dstRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                } else {
                    dstRect = { 
                        static_cast<int>(x * TILE_SIZE), 
                        static_cast<int>((static_cast<int>(grid.size()) - y - 1) * TILE_SIZE), 
                        TILE_SIZE, 
                        TILE_SIZE 
                    };
                    
                }
                SDL_RenderCopy(gRenderer, tileTex, nullptr, &dstRect);
            }
        }
    }
    // render checkpoints
    levelManager->renderCheckpoint(gRenderer);
    // Render the player
    player->render(gRenderer);
}


void Game::controlFrameRate(Uint32 frameStart, int frameDelay) {
    int frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < frameDelay) {
        SDL_Delay(frameDelay - frameTime);
    }
}


void Game::run() {
    const int FRAME_DELAY = 1000 / 60; // 60 FPS
    Uint32 lastTime = SDL_GetTicks();
    levelManager->LoadLevel();
    levelManager->loadCheckPoint(levelManager->getCurrentLevel());

    while (!quit) {
        input();  
        Uint32 frameStart = SDL_GetTicks();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (!pauseGame) {
            update(deltaTime);  
            render();           
        }
        else {
            renderPauseMenu(gRenderer);
        }
        controlFrameRate(frameStart, FRAME_DELAY);
        SDL_RenderPresent(gRenderer);
    }
}


void Game::renderBackground(SDL_Renderer* renderer, SDL_Texture* gLayerX, float offsetX, float offsetY, bool isVertical){
    if (gLayerX != nullptr) {
        SDL_Rect bgRect1, bgRect2;

        if (!isVertical) {
            // Standard horizontal scrolling
            bgRect1 = { static_cast<int>(offsetX), 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            bgRect2 = { static_cast<int>(offsetX + SCREEN_WIDTH), 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        } else {
            // Vertical scrolling
            bgRect1 = { 0, static_cast<int>(offsetY), SCREEN_WIDTH, SCREEN_HEIGHT };
            bgRect2 = { 0, static_cast<int>(offsetY + SCREEN_HEIGHT), SCREEN_WIDTH, SCREEN_HEIGHT };
        }

        SDL_RenderCopy(renderer, gLayerX, NULL, &bgRect1);
        SDL_RenderCopy(renderer, gLayerX, NULL, &bgRect2);
    }
}

void Game::close() {

    SDL_DestroyTexture(gLayer1);
    SDL_DestroyTexture(gLayer2);
    SDL_DestroyTexture(gLayer3);
    SDL_DestroyTexture(gLayer4);
    gLayer1 = gLayer2 = gLayer3 = gLayer4 = nullptr;
    
    if (player) {
        delete player;
        player = nullptr;
    }
    //Tile::cleanUp();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;
    
    IMG_Quit();
    SDL_Quit();
}

void Game::renderPauseMenu(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_Rect overlay = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_Rect pauseBox = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &pauseBox);

}