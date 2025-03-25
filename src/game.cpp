#include "game.h"

Game::Game()
    : gWindow(nullptr), gRenderer(nullptr), gLayer1(nullptr), gLayer2(nullptr), gLayer3(nullptr),
      offset1(0.0f), offset2(0.0f), offset3(0.0f), player(nullptr){
}

Game::~Game() {
    close();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
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

void Game::input(){
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        player->handleEvent(e);
    }
}

void Game::update(float deltaTime) {
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
    player->update(deltaTime);
    player->updateAnimation(deltaTime);

    // Handle level transition
    if (player->reachedExit()) {
        levelManager->NextLevel();
        player->reset();
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

    // Render the player
    player->render(gRenderer);

    SDL_RenderPresent(gRenderer);
}


void Game::controlFrameRate(Uint32 frameStart, int frameDelay) {
    int frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < frameDelay) {
        SDL_Delay(frameDelay - frameTime);
    }
}

void Game::run() {
    const int FRAME_DELAY = 1000 / 60; // Target 60 FPS
    Uint32 lastTime = SDL_GetTicks();

    // Load the current level grid once at start
    levelManager->LoadLevel();

    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();

        // 1. Process input events.
        input();

        // 2. Compute delta time.
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // 3. Update game state.
        update(deltaTime);

        // 4. Render current frame.
        render();

        // 5. Frame rate control.
        controlFrameRate(frameStart, FRAME_DELAY);
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