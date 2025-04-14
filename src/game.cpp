#include "game.h"

GameState gameState = STATE_MENU;
GameState prevState;

Game::Game()
    : gWindow(nullptr), gRenderer(nullptr), player(nullptr), pauseGame(false){
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

    settingsMenu.setBackground("assets/1.png", gRenderer);
    int musicButton = settingsMenu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 35, 288, 64, "Music");
    int backButton = settingsMenu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 35, 288, 64, "Back");

    settingsMenu.setButtonTexture(musicButton, "assets/Buttons/button.png", gRenderer);
    settingsMenu.setButtonTexture(backButton, "assets/Buttons/button.png", gRenderer);
    settingsMenu.loadArrow("assets/Buttons/select_button.png", gRenderer);

    pauseMenu.setBackground("assets/1.png", gRenderer);
    int continueButton = pauseMenu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 70, 288, 64, "Continue");
    int settings = pauseMenu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 288, 64, "Settings");
    int quitButton = pauseMenu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 70, 288, 64, "Quit");

    pauseMenu.setButtonTexture(continueButton, "assets/Buttons/button.png", gRenderer);
    pauseMenu.setButtonTexture(settings, "assets/Buttons/button.png", gRenderer);
    pauseMenu.setButtonTexture(quitButton, "assets/Buttons/button.png", gRenderer);
    pauseMenu.loadArrow("assets/Buttons/select_button.png", gRenderer);

    menu.setBackground("assets/1.png", gRenderer);
    start_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 70, 288, 64, "Start");
    settings_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 288, 64, "Settings");
    exit_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 70, 288, 64, "Quit");
    
    menu.setButtonTexture(start_button, "assets/Buttons/button.png", gRenderer);
    menu.setButtonTexture(settings_button,  "assets/Buttons/button.png", gRenderer);
    menu.setButtonTexture(exit_button,  "assets/Buttons/button.png", gRenderer);
    menu.loadArrow("assets/Buttons/select_button.png", gRenderer);    

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
        return false;
    }

    if(!menu.loadSound() || !settingsMenu.loadSound() || !pauseMenu.loadSound()){
        std::cerr << "Failed to load player sound effects!\n";
        return false;
    }

    player = new Player(
        gRenderer,    
        50.0f,        // posX (Starting X position)
        300.0f,       // posY (Starting Y position)
        6,            // normalSpeed
        10,           // dashSpeed
        0.0f,         // velY
        false,        // isJumping
        false,        // isFalling
        0,            // dashStartTime
        0             // dashCooldownTime
    );

    if (!player->loadSound()){
        std::cerr << "Failed to load player sound effects!\n";
        return false;
    }

    levelManager = new LevelManager(gRenderer, gWindow);
    seeker = new Seeker(gRenderer, levelManager);
    seeker->loadTexture("assets/seeker.png");
    seeker->loadCoinTexture("assets/coin.png");
    seeker->spawnCoin(5);

    far = loadTexture("assets/parallax/layer1.png");
    mid = loadTexture("assets/parallax/layer2.png");
    mid_near = loadTexture("assets/parallax/layer3.png");
    near = loadTexture("assets/parallax/layer4.png");

    music = Mix_LoadMUS("assets/sound/vanishinghope.wav");
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << '\n';
        return false;
    }

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

void Game::input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        if (gameState == STATE_MENU) {
            menu.updateWidget(event, gameState);
            thegreatReset();
        }
        else if(gameState == STATE_SETTINGS){
            settingsMenu.updateWidget(event, gameState);
            if(gameState == STATE_MENU || gameState == STATE_PAUSE){
                gameState = prevState;
            }
        }
        else if(gameState == STATE_PAUSE){
            pauseMenu.updateWidget(event, gameState);
            if(gameState == STATE_GAME){
                pauseGame = false;
            }
        }
        else if(gameState == STATE_WIN){
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN){
                gameState = STATE_MENU;
            }
        }
        else if(gameState == STATE_GAME) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                pauseGame = !pauseGame;  
                gameState = STATE_PAUSE;
                
                if(pauseGame && player){
                    player->stopMovement();
                }
            }

            if (!pauseGame && player) { 
                player->handleEvent(event, pauseGame);
            }

            if (dialogue.isActive()) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_z) {
                        dialogue.skipOrNext();
                    } else {
                        dialogue.handleKey(event.key.keysym.sym);
                    }
                }
            }
            
        }
        else if(gameState == STATE_DEAD){
            Uint32 timeofDeath = SDL_GetTicks() - deathTimer;
            if(timeofDeath >= 1000){
                waitKey = true;
            }
            if(waitKey && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN){
                gameState = STATE_MENU;
            }
            player->reset(0);
        }
        if (gameState == STATE_END) {
            quit = true;
        }
    }
}

void Game::update(float deltaTime) {
    if(gameState == STATE_MENU){
        menu.updateBackground(deltaTime);
    }
    else if(gameState == STATE_SETTINGS){
        settingsMenu.updateBackground(deltaTime);
    }
    else if(gameState == STATE_PAUSE){
        pauseMenu.updateBackground(deltaTime);
        if (gameState == STATE_GAME) {
            pauseGame = false;
        }
    }
    else if(gameState == STATE_GAME) {
        if(pauseGame) return;

        std::vector<std::vector<int>> path = seeker->loadCSV(levelManager->getLevelCSV());
        if(!dialogue.isActive()){
            player->update(deltaTime, pauseGame, levelManager->isSpecialLevel(), levelManager->GetLevelData(), levelManager->getCurrentLevel());
            seeker->update(deltaTime, path, player->getRect(), levelManager->getCurrentLevel());
            player->updateAnimation(deltaTime);
        }
        if (!hasTrigger) {
            SDL_Rect playerRect = player->getRect();
            int tileX = playerRect.x / TILE_SIZE;
            int tileY = playerRect.y / TILE_SIZE;

            std::vector<std::vector<Tile>> grid = levelManager->GetLevelData();

            if (tileY >= 0 && tileY < grid.size() &&
                tileX >= 0 && tileX < grid[0].size()) {

                int tileType = grid[tileY][tileX].type;

                if (tileType == DIALOGUE_TRIGGER_ANOTHER && !hasFirstDialogue) {
                    hasFirstDialogue = true;
                    dialogue.trigger({
                        "Greetings, today is such a lovely day!",
                        "BTW, you can press Z to skip what I'll say",
                        "Bach is the creator of me, he wants to tell you something",
                        "There are 2 modes: flashlight and no flashlight",
                        "What do you want?",
                        "Okay now have fun"
                    }, true);
                }
                
                int temp_level = levelManager->getCurrentLevel();
                if (tileType == DIALOGUE_TRIGGER && !hasSecondDialogue && temp_level != 0) {
                    hasSecondDialogue = true;
                    dialogue.trigger({
                        "Did you have fun ?",
                        "Well, hope you did because this is the end of the game",
                        "See you again..."
                    }, false);
                }
                if(tileType == 513){
                    gameState = STATE_WIN;
                }
            }
        }
        dialogue.update(deltaTime);

        if (player->reachedExit(levelManager->isSpecialLevel(), levelManager->getCurrentLevel())) {
            levelManager->NextLevel();
            player->reset(levelManager->getCurrentLevel());
            seeker->respawn();
        }

        if(player->isDead(levelManager->getCurrentLevel())){ 
            gameState = STATE_DEAD;
            deathTimer = SDL_GetTicks();
            waitKey = false;
        }

        if(seeker->seekerCollidePlayer(player->getRect())){
            gameState = STATE_DEAD;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);
    if(gameState == STATE_MENU){
        SDL_Rect screen = {SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 - 180, 180, 64};
        menu.renderBackground(gRenderer);
        levelManager->renderText("MOONLIGHT", font, screen, {0,0,139, 180});
        menu.button_render(gRenderer, font);
        return;
    }
    else if(gameState == STATE_SETTINGS){
        settingsMenu.renderBackground(gRenderer);
        settingsMenu.button_render(gRenderer, font);
        return;
    }
    else if(gameState == STATE_PAUSE){
        pauseMenu.renderBackground(gRenderer);
        pauseMenu.button_render(gRenderer, font);
        return;
    }
    else if (gameState == STATE_DEAD) {        
        std::string message = waitKey ? "You Died! Press Enter to return to menu." : "You Died...";
        int screenH, screenW;
        if(levelManager->isSpecialLevel()){
            screenW = PORTRAIT_WIDTH;
            screenH = PORTRAIT_HEIGHT;
        }
        else{
            screenW = LANDSCAPE_WIDTH;
            screenH = LANDSCAPE_HEIGHT;
        }
        SDL_Rect screen = {0,0, screenW + 300, screenH};
        levelManager->renderText(message, font ,screen, {0,0,0,180});
    }
    else if(gameState == STATE_WIN){
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
    
        SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 - 35, 288, 64};
        SDL_Rect messageRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 35, 300, 64};
    
        levelManager->renderText("YOU WIN!", font, titleRect, {0, 0, 139, 255}); 
        levelManager->renderText("Press Enter to return.", font, messageRect, {0, 0, 139, 255});
    }
    else if(gameState == STATE_GAME) {
        float playerX = player->getX();
        int levelNum = levelManager->getCurrentLevel();

        if(levelNum != 4){
            static float farX = 0.0f;
            static float midX = 0.0f;
            farX -= 0.5f;
            midX -= 0.8f;

            if(farX <= -SCREEN_WIDTH) farX += SCREEN_WIDTH;
            if(midX <= -SCREEN_WIDTH) midX += SCREEN_WIDTH;

            int mid_nearX = static_cast<int>(-playerX * 0.34f);
            int nearX = static_cast<int>(-playerX * 0.7f);

            SDL_Rect dest = {0,0, LANDSCAPE_WIDTH, LANDSCAPE_HEIGHT};
            for (int i = -1; i <= 1; ++i) {
                dest.x = static_cast<int>(farX + i * SCREEN_WIDTH);
                SDL_RenderCopy(gRenderer, far, nullptr, &dest);
            }
        
            for (int i = -1; i <= 1; ++i) {
                dest.x = static_cast<int>(midX + i * SCREEN_WIDTH);
                SDL_RenderCopy(gRenderer, mid, nullptr, &dest);
            }
        
            for (int i = -1; i <= 1; ++i) {
                dest.x = mid_nearX + i * SCREEN_WIDTH;
                SDL_RenderCopy(gRenderer, mid_near, nullptr, &dest);
            }
        
            for (int i = -1; i <= 1; ++i) {
                dest.x = nearX + i * SCREEN_WIDTH;
                SDL_RenderCopy(gRenderer, near, nullptr, &dest);
            }
        }
        
        std::vector<std::vector<Tile>> grid = levelManager->GetLevelData();
        for (int y = 0; y < grid.size(); y++) {
            for (int x = 0; x < grid[y].size(); x++) {
                SDL_Texture* tileTex = levelManager->GetTexture(grid[y][x].type);
                if (tileTex) {
                    SDL_Rect dstRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    SDL_RenderCopy(gRenderer, tileTex, nullptr, &dstRect);
                }
            }
        }
        
        player->render(gRenderer);
        seeker->render(gRenderer, player->getRect(), levelManager->getCurrentLevel());

        if(levelManager->isSpecialLevel()){
            SDL_Rect scoreBox = {PORTRAIT_WIDTH - 20, PORTRAIT_HEIGHT - 20, 20, 20};
            levelManager->renderText(seeker->getScore(), font, scoreBox, {0,0,0, 200});
        }
        else{
            SDL_Rect scoreBox = {LANDSCAPE_WIDTH - 100, LANDSCAPE_HEIGHT - 100, 80, 80};
            levelManager->renderText(seeker->getScore(), font, scoreBox, {0,0,0, 200});
        }
        
        if(dialogue.isModeEnabled()){
            int lightX = player->getX() + PLAYER_WIDTH / 3;
            int lightY = player->getY() + PLAYER_HEIGHT / 3;
            int lightRadius = 150;  
            if(!levelManager->isSpecialLevel()){
                player->flashLight(gRenderer, lightX, lightY, lightRadius, LANDSCAPE_WIDTH, LANDSCAPE_HEIGHT, true);
            }
            else{
                player->flashLight(gRenderer, lightX, lightY, lightRadius, LANDSCAPE_HEIGHT, LANDSCAPE_WIDTH, true);
            }
            
            
        }

        dialogue.render(gRenderer, font);
    }
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
    if(music){
        Mix_PlayMusic(music, -1);
    }

    while (!quit) {
        input();  
        Uint32 frameStart = SDL_GetTicks();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
    
        if (gameState == STATE_MENU || gameState == STATE_GAME || gameState == STATE_SETTINGS || gameState == STATE_DEAD) {
            update(deltaTime);  
            render();           
        }
        else if (gameState == STATE_PAUSE) {
            pauseMenu.updateBackground(deltaTime);
            pauseMenu.renderBackground(gRenderer);
            pauseMenu.button_render(gRenderer, font);
        }
    
        controlFrameRate(frameStart, FRAME_DELAY);
        SDL_RenderPresent(gRenderer);
    }
}

void Game::thegreatReset(){
    if (player) {
        player->reset(0);  
    }

    if (seeker) {
        seeker->respawn(); 
        seeker->spawnCoin(5);
    }

    if (levelManager) {
        levelManager->reset();  
    }

    dialogue.setMode(false);

    hasFirstDialogue = false;
    hasSecondDialogue = false;
    hasTrigger = false;

    pauseGame = false;
    waitKey = false;
}

void Game::close() {
    if (player) {
        delete player;
        player = nullptr;
    }

    if(seeker){
        delete seeker;
        seeker = nullptr;
    }

    if(levelManager){
        delete levelManager;
        levelManager = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    SDL_DestroyTexture(far);
    SDL_DestroyTexture(mid);
    SDL_DestroyTexture(mid_near);
    SDL_DestroyTexture(near);

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;
    
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();
}