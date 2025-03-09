#include "game.h"

Game::Game()
    : gWindow(nullptr), gRenderer(nullptr), gLayer1(nullptr), gLayer2(nullptr), gLayer3(nullptr),
      offset1(0.0f), offset2(0.0f), offset3(0.0f), player(nullptr), currentMap(nullptr){
}

Game::~Game() {
    close();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }
    
    gWindow = SDL_CreateWindow("Parallax Scrolling Game",
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
    currentMap = Map(gRenderer);
    currentMap.loadTileTexture();
    currentMap.loadMap("assets/TileMap/chunk1.csv", 40, 22);
    
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

    // Create the playable character
    player = new Player(gRenderer);
    return true;
}

void Game::run() {
    bool quit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();
    
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            player->handleEvent(e);
        }
        
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // ------ PLAYER ------- //
        player->update(deltaTime);
        player->updateAnimation(deltaTime);

        offset1 -= baseSpeed * 0.2f * deltaTime;
        offset2 -= baseSpeed * 0.5f * deltaTime;
        offset3 -= baseSpeed * 1.0f * deltaTime;
        offset4 -= baseSpeed * 1.2f * deltaTime;

        if (offset1 <= -SCREEN_WIDTH) offset1 = 0;
        if (offset2 <= -SCREEN_WIDTH) offset2 = 0;
        if (offset3 <= -SCREEN_WIDTH) offset3 = 0;
        if (offset4 <= -SCREEN_WIDTH) offset4 = 0;

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        renderBackground(gRenderer, gLayer1, offset1);
        renderBackground(gRenderer, gLayer2, offset2);
        renderBackground(gRenderer, gLayer3, offset3);
        renderBackground(gRenderer, gLayer4, offset4);

        // ------ MAPS --------//
        currentMap.drawMap();
        player->render(gRenderer);

        SDL_RenderPresent(gRenderer);
    }
}

void Game::renderBackground(SDL_Renderer* renderer, SDL_Texture* gLayerX, float offsetX){
    if(gLayerX != nullptr){
        SDL_Rect bgRect1 = {
            static_cast<int> (offsetX),
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
        SDL_Rect bgRect2 = {
            static_cast<int> (offsetX + SCREEN_WIDTH),
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
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